#include "netio/netio.hpp"
#include "utility.hpp"

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/signal.h>

//#define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[eventloop@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif
#define INFO_LOG( ... ) do { printf("[posix@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)

ssize_t safe_write(int fd, const void* buf, size_t bufsz);


netio::event_loop::event_loop()
{
    epollfd = epoll_create(1024); // size argument ignored, see EPOLL_CREATE(2)
    DEBUG_LOG("EPOLLFD is %d", epollfd);
    if(epollfd == -1)
    {
        raise_errno_exception();
    }
}


netio::event_loop::~event_loop()
{
    close(epollfd);
}


static void
process_event(netio::event_loop::context* ctx)
{
  DEBUG_LOG("process_event: ctx: 0x%x    fn: 0x%x     fd: %d      data: 0x%x", ctx, ctx->fn, ctx->fd, ctx->data);
    if(ctx->fn)
        ctx->fn(ctx->fd, ctx->data);
}


unsigned
netio::event_loop::wait_for_events(int epollfd, unsigned timeout_millisecs)
{
    //std::lock_guard<netio::spinlock> g(lock);

    DEBUG_LOG("waiting for events");
    const unsigned MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];

    int nevents = epoll_wait(epollfd, events, MAX_EVENTS, timeout_millisecs);

    if(nevents == -1)
    {
        if(errno == EINTR)
        {
            return 0;
        }
        netio::raise_errno_exception();
    }

    DEBUG_LOG("%d events to process", nevents);

    /* This container is here to keep track of the contexts that are destroyed
     * while processing these set of events. Calling unregister_fd() makes sure
     * that subsequent calls to epoll_wait won't return any events for those
     * files descriptors. But we might sill have a copy of the context pointer
     * in events[].
     * For example, this case happens when timers and signals are deleted while
     * processing incoming data on a socket (connection is closed, associated
     * timers and signals are deleted, but they might have already be part of
     * events[]).
     */
    unregistered_contexts.clear();

    for(int i=0; i<nevents; i++)
    {
        DEBUG_LOG("event type: %x", events[i].events);
        netio::event_loop::context* ctx = (netio::event_loop::context*)(events[i].data.ptr);

        if (unregistered_contexts.find(ctx) != unregistered_contexts.end()) {
            DEBUG_LOG("skipping event for a deleted context: event type: %x, associated with context: %p.",
                events[i].events, (void*)ctx);
            continue;
        }

        process_event(ctx);

        if (unregistered_contexts.find(ctx) != unregistered_contexts.end()) {continue;}

        if(events[i].events & EPOLLRDHUP)
        {
            DEBUG_LOG("EPOLLRDHUP, closing ctx.fd %d", ctx->fd);
            this->unregister_fd(ctx);
            int ret = close(ctx->fd);
            if( ret < 0 ){DEBUG_LOG("Caught error %s closing ctx.fd %d", strerror(errno), ctx->fd);}
        }
    }
    return nevents;
}


void
netio::event_loop::run_forever()
{
    const unsigned TIMEOUT_MILLISECS = 100;
    running.store(true);
    while(running.load())
    {
        wait_for_events(epollfd, TIMEOUT_MILLISECS);
    }
}


static unsigned long long
now_millisecs()
{
    struct timespec t;
    if(-1 == clock_gettime(CLOCK_MONOTONIC_COARSE, &t))
    {
        netio::raise_errno_exception();
    }
    return t.tv_sec*1000 + t.tv_nsec/(1000*1000);
}


void
netio::event_loop::run_for(unsigned long long millisecs)
{
    unsigned long long tp = now_millisecs();
    running.store(true);
    while(running.load())
    {
        wait_for_events(epollfd, millisecs);
        if((now_millisecs() - tp) > millisecs)
        {
            running.store(false);
            return;
        }
    }
}


void
netio::event_loop::run_one()
{
    DEBUG_LOG("process single event");
    const unsigned TIMEOUT_MILLISECS = 10;
    while(wait_for_events(epollfd, TIMEOUT_MILLISECS) <= 0)
        ;
}


void
netio::event_loop::stop()
{
    DEBUG_LOG("stopping event loop");
    running.store(false);
}


static void
register_fd(int epfd, netio::event_loop::context* ctx, int flags)
{
    struct epoll_event ev;
    ev.events = flags;
    ev.data.ptr = ctx;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ctx->fd, &ev))
    {
        netio::raise_errno_exception();
    }
}


void
netio::event_loop::register_read_fd(context* ctx)
{
    register_fd(epollfd, ctx, EPOLLIN | EPOLLRDHUP);
}


void
netio::event_loop::register_write_fd(context* ctx)
{
    register_fd(epollfd, ctx, EPOLLOUT | EPOLLRDHUP);
}


void
netio::event_loop::register_read_fd_et(context* ctx)
{
    register_fd(epollfd, ctx, EPOLLIN | EPOLLRDHUP | EPOLLET);
}


void
netio::event_loop::register_write_fd_et(context* ctx)
{
    register_fd(epollfd, ctx, EPOLLOUT | EPOLLRDHUP | EPOLLET);
}


void
netio::event_loop::unregister_fd(context* ctx)
{
    /* ev argument is ignored, but required due to a bug in kernel
       versions < 2.6.9, see EPOLL_CTL(2) */
    struct epoll_event ev;

    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, ctx->fd, &ev))
    {
        //EINVAL happens when epollfd and ctx->fd are the same
        if ((errno != ENOENT) && (errno != EBADF) && (errno != EINVAL))
            raise_errno_exception();
    }

    unregistered_contexts.insert(ctx);
}


bool
netio::event_loop::is_running() const
{
    return running.load();
}


netio::timer::timer(event_loop* evloop, std::function<void(void*)> fn, void* data)
{
    this->evloop = evloop;
    ctx.fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    DEBUG_LOG("TIMER FD %d", ctx.fd);
    if(ctx.fd == -1)
    {
        raise_errno_exception();
    }
    ctx.fn = [fn, data](int fd, void*)
    {
        DEBUG_LOG("timer event on FD %d", fd);
        uint64_t buf;
        if(8 != read(fd, &buf, 8))
            DEBUG_LOG("Did not read 8 bytes");
        fn(data);
    };
    this->evloop->register_read_fd(&ctx);
}


netio::timer::~timer()
{
    stop();
    evloop->unregister_fd(&ctx);
    close(ctx.fd);
}


static void
set_timerfd(int fd, unsigned millisecs)
{
    struct itimerspec it;
    it.it_interval.tv_sec = millisecs / 1000;
    it.it_interval.tv_nsec = (millisecs % 1000) * 1000 * 1000;
    it.it_value.tv_sec = millisecs / 1000;
    it.it_value.tv_nsec = (millisecs % 1000) * 1000 * 1000;
    if(timerfd_settime(fd, 0, &it, NULL))
        netio::raise_errno_exception();
}


void
netio::timer::start(unsigned millisecs)
{
    DEBUG_LOG("SET TIMER FD %d", ctx.fd);
    set_timerfd(ctx.fd, millisecs);
}


void
netio::timer::stop()
{
    set_timerfd(ctx.fd, 0);
}


netio::signal::signal(event_loop* evloop, std::function<void(void*)> fn, void* data)
{
    this->evloop = evloop;
    this->fn = fn;
    ctx.fd = eventfd(0, EFD_NONBLOCK);
    DEBUG_LOG("Signal on FD %d", ctx.fd);
    if(ctx.fd == -1)
    {
        raise_errno_exception();
    }
    ctx.fn = [fn, data](int fd, void*)
    {
        DEBUG_LOG("signal event on FD %d", fd);
        uint64_t buf;
        if(8 != read(fd, &buf, 8))
            DEBUG_LOG("Did not read 8 bytes");
        fn(data);
    };
    this->evloop->register_read_fd_et(&ctx);
}


netio::signal::~signal()
{
    evloop->unregister_fd(&ctx);
    close(ctx.fd);
}


void
netio::signal::fire()
{
    uint64_t buf = 1;
    //write(ctx.fd, &buf, 8);
    safe_write(ctx.fd, &buf, 8);
}

//Read https://riptutorial.com/posix/example/17424/handle-sigpipe-generated-by-write---in-a-thread-safe-manner
ssize_t safe_write(int fd, const void* buf, size_t bufsz)
{
    sigset_t sig_block, sig_restore, sig_pending;

    sigemptyset(&sig_block);
    sigaddset(&sig_block, SIGPIPE);

    /* Block SIGPIPE for this thread.
     *
     * This works since kernel sends SIGPIPE to the thread that called write(),
     * not to the whole process.
     */
    if (pthread_sigmask(SIG_BLOCK, &sig_block, &sig_restore) != 0) {
        return -1;
    }

    /* Check if SIGPIPE is already pending.
     */
    int sigpipe_pending = -1;
    if (sigpending(&sig_pending) != -1) {
        sigpipe_pending = sigismember(&sig_pending, SIGPIPE);
    }

    if (sigpipe_pending == -1) {
        pthread_sigmask(SIG_SETMASK, &sig_restore, NULL);
        return -1;
    }

    ssize_t ret = write(fd, buf, bufsz);

    int errsv = errno;
    //INFO_LOG("Caught error %s on fd %d", strerror(errsv), fd);
    //while ((ret = write(fd, buf, bufsz)) == -1) {
    //    if (errno != EINTR)
    //        break;
    //}

    /* Fetch generated SIGPIPE if write() failed with EPIPE.
     *
     * However, if SIGPIPE was already pending before calling write(), it was
     * also generated and blocked by caller, and caller may expect that it can
     * fetch it later. Since signals are not queued, we don't fetch it in this
     * case.
     */
    if (ret == -1 && errsv == EPIPE && sigpipe_pending == 0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 0;

        int sig;
        while ((sig = sigtimedwait(&sig_block, 0, &ts)) == -1) {
            if (errno != EINTR)
                break;
        }
    }

    pthread_sigmask(SIG_SETMASK, &sig_restore, NULL);
    return ret;
}
