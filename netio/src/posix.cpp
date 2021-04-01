#include "posix.hpp"
#include "backend.hpp"
#include "utility.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXCONN (1024)


//#define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[posix@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif
#define INFO_LOG( ... ) do { printf("[posix@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
//#define DEBUG_LOG( ... ) do { printf("[posix@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)

//Asynchronous FD writer or not?
//#define AFDW


#ifdef AFDW
static void make_socket_non_blocking (int sfd);


netio::async_fd_writer::async_fd_writer()
{
  fd = -1;
}


netio::async_fd_writer::async_fd_writer(int fd)
{
  this->fd = fd;
  this->connection_dead = false;
  make_socket_non_blocking(fd);
  DEBUG_LOG("async_fd_writer: opened for FD %d", fd);
}


netio::async_fd_writer& netio::async_fd_writer::operator=(const netio::async_fd_writer& other)
{
    DEBUG_LOG("async_fd_writer: operator= %d %d", fd, other.fd);

    if(this != &other) {
        this->fd = other.fd;
        this->current_buffer = other.current_buffer;
        this->size = other.size;
        this->bytes_written = other.bytes_written;
        this->connection_dead = other.connection_dead;
    }
    return *this;
}

netio::async_fd_writer::~async_fd_writer()
{}


void netio::async_fd_writer::add_buffer(netio::reusable_buffer* buffer)
{
  if(connection_dead) {
    buffer->release();
    throw std::runtime_error("connection died");
  }
  DEBUG_LOG("async_fd_writer: adding buffer 0x%x", buffer);
  buffers_to_send.push(buffer);
  this->send();
}


void netio::async_fd_writer::send()
{
  std::lock_guard<std::mutex> lock(mtx);

  DEBUG_LOG("async_fd_writer: send() FD: %d", fd);
  while(true) {
    DEBUG_LOG("HELLO: %d", fd);

    if(!current_buffer) {
      if(!buffers_to_send.try_pop(current_buffer)) {
        // no buffers to send
        DEBUG_LOG("async_fd_writer: no more buffers to send");
        return;
      }
      bytes_written = 0;
      size = current_buffer->buffer()->pos();
      DEBUG_LOG("async_fd_writer: fetched new buffer size: %d", size);
    }
    if(size==0){
      current_buffer->release();
      current_buffer = NULL;
      return;
    }
    DEBUG_LOG("bytes written %d size: %d", bytes_written, size);
    while(bytes_written < size)
    {
        int result = ::send(fd, (char*)current_buffer->buffer()->data() + bytes_written,
                          size-bytes_written, MSG_NOSIGNAL);
        DEBUG_LOG("result %d",result);
        if(result == -1)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
              DEBUG_LOG("async_fd_writer: EAGAIN");
              return;
            } else {
              DEBUG_LOG("connection died");
              connection_dead = true;
              current_buffer->release();
              while(buffers_to_send.try_pop(current_buffer)) {
                current_buffer->release();
              }
              return;
            }
        } else {
            bytes_written += result;
            DEBUG_LOG("async_fd_writer:  written %d bytes", bytes_written);
            if(bytes_written >= size) {
              DEBUG_LOG("async_fd_writer: buffer sent, releasing");
              current_buffer->release();
              current_buffer = NULL;
            }
        }
    }
  } //end of while true
}


void netio::async_fd_writer::flush_all_blocking()
{
    DEBUG_LOG("async_fd_writer: blocking until all data is sent");
    if(connection_dead)
      return;
    while(!buffers_to_send.empty() || current_buffer) {
      send();
    }
}


static void async_write_to_fd(int fd, void* p)
{
    netio::async_fd_writer* fdwriter = (netio::async_fd_writer*)p;
    fdwriter->send();
}
#endif

netio::posix_send_socket::posix_send_socket(event_loop* evloop, sockcfg cfg)
    : backend_send_socket(evloop, cfg)
{
}


netio::posix_send_socket::~posix_send_socket()
{
    disconnect();
}


void
netio::posix_send_socket::connect(const endpoint& ep)
{
    char port[32];
    snprintf(port, 32, "%d", ep.port());


    ctx.fd = ::socket(ep.sockaddr()->sa_family, SOCK_STREAM, 0);

    if (ctx.fd == -1)
    {
        raise_errno_exception();
    }

    if (::connect(ctx.fd, ep.sockaddr(), sizeof(struct sockaddr_storage)) != 0)
    {
        DEBUG_LOG("Posix_send_socket, closing ctx.fd %d", ctx.fd);
        close(ctx.fd);
        THROW_WITH_MSG(std::runtime_error, "could not connect to endpoint "
                       << ep.address()  << ":" << port);
    }

    int state = 1;
    setsockopt(ctx.fd, IPPROTO_TCP, TCP_NODELAY, &state, sizeof(state));
    state = 1;
    setsockopt(ctx.fd, SOL_SOCKET, SO_KEEPALIVE, &state, sizeof(state));

#ifdef AFDW
    //async_fd_writer
    fd_writer = async_fd_writer(ctx.fd);
    // We register the FD in the eventloop with an empty callback
    // so a peer disconnect is handled (EPOLLRDHUP)
    ctx.data = &fd_writer;
    ctx.fn = async_write_to_fd;
#else
    ctx.data = nullptr;
    ctx.fn = nullptr;
#endif

    evloop->register_write_fd_et(&ctx);

    if(on_connection_opened)
        on_connection_opened();
    status = OPEN;
}


void
netio::posix_send_socket::disconnect()
{
#ifdef AFDW
  DEBUG_LOG("posix_send_socket::disconnect");
  try {
    fd_writer.flush_all_blocking();
  } catch(...) {
    DEBUG_LOG("caught exception while flushing");
  }
#endif
  evloop->unregister_fd(&ctx);
  close(ctx.fd);
  status = CLOSED;
  if(on_connection_closed)
      on_connection_closed();
}


#ifndef AFDW
static void
write_to_fd(int fd, const char* buffer, size_t size)
{
    size_t bytes_written = 0;
    while(bytes_written < size)
    {
        int result = send(fd, (char*)buffer + bytes_written,
                          size-bytes_written, MSG_NOSIGNAL);
        if(result == -1)
        {
            netio::raise_errno_exception();
        }
        bytes_written += result;
    }
}
#endif



void
netio::posix_send_socket::send_buffer(netio::reusable_buffer* buffer)
{
    DEBUG_LOG("POSIX: send a reusable buffer");
    try
    {
#ifdef AFDW
        fd_writer.add_buffer(buffer);
#else
        write_to_fd(ctx.fd, buffer->buffer()->data(), buffer->buffer()->pos());
        DEBUG_LOG("written to FD, releasing buffer now");
        buffer->release();
#endif
    }
    catch(std::system_error& e)
    {
        // Connection was closed (or is otherwise broken)
        DEBUG_LOG("There was an exception: system_error %s", e.what());
        disconnect();
    }
    catch(...)
    {
        DEBUG_LOG("There was an unknown exception");
    }
}



static void
make_socket_non_blocking (int sfd)
{
    int flags, s;

    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1)
    {
        netio::raise_errno_exception();
    }

    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1)
    {
        netio::raise_errno_exception();
    }
}


static int
create_and_bind (const struct sockaddr* sa)
{
    int sfd;

    sfd = socket (sa->sa_family, SOCK_STREAM, 0);
    DEBUG_LOG("NEW SOCKET FD %d", sfd);
    if (sfd == -1)
    {
        throw std::runtime_error("Could not create socket");
    }

    int yes=1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        throw std::runtime_error("setsockopt failed");
    }

    if (bind(sfd, sa, sizeof(struct sockaddr_storage)) != 0)
    {
        close (sfd);
        throw std::runtime_error("Could not bind");
    }

    make_socket_non_blocking(sfd);
    return sfd;
}


void
netio::posix_listen_socket::accept_connections()
{
    while (1)
    {
        struct sockaddr in_addr;
        socklen_t in_len;
        int infd;

        in_len = sizeof in_addr;
        infd = accept (ctx.fd, &in_addr, &in_len);
        DEBUG_LOG("RECV_socket FD %d", infd);
        if (infd == -1)
        {
            if ((errno == EAGAIN) ||
                    (errno == EWOULDBLOCK))
            {
                /* We have processed all incoming
                   connections. */
                break;
            }
            else
            {
                netio::raise_errno_exception();
            }
        }

        /* Make the incoming socket non-blocking and add it to the
           list of fds to monitor. */
        make_socket_non_blocking (infd);
        try {
            netio::posix_recv_socket* socket = new netio::posix_recv_socket(evloop, this, infd);
            if(on_connected) on_connected(*socket);
        } catch (const std::exception& e) {
	    printf("Ignored Exception (Bad File Descriptor) %s\n", e.what());
            fflush(stdout);
            break;
        }
    }
}


netio::posix_listen_socket::posix_listen_socket(event_loop* evloop, netio::endpoint ep,
                                                netio::context* c, sockcfg cfg)
    : backend_listen_socket(evloop, ep, c, cfg)
{
    ctx.fd = 0;
}


netio::posix_listen_socket::~posix_listen_socket()
{
    if(ctx.fd)
    {
        evloop->unregister_fd(&ctx);
        close(ctx.fd);
    }
}


void
netio::posix_listen_socket::listen()
{
    ctx.fd = create_and_bind(this->ep.sockaddr());
    if(::listen(ctx.fd, MAXCONN) == -1)
    {
        raise_errno_exception();
    }

    ctx.fn = [this](int sfd, void*)
    {
        accept_connections();
    };
    evloop->register_read_fd(&ctx);
}


netio::endpoint
netio::posix_listen_socket::endpoint() const
{
    struct sockaddr_in local_socket_addr;
    socklen_t local_socket_addr_len = sizeof local_socket_addr;
    if(-1 == getsockname(this->ctx.fd, (struct sockaddr*)&local_socket_addr, &local_socket_addr_len))
    {
        netio::raise_errno_exception();
    }

    char* addr = inet_ntoa(local_socket_addr.sin_addr);
    unsigned short port = ntohs(local_socket_addr.sin_port);
    return netio::endpoint(addr, port);
}

void
netio::posix_end_processing_and_close_socket(netio::posix_recv_socket* socket)
{
  socket->evloop->unregister_fd(&socket->ctx);

  if(socket->current_page != nullptr)
  {
      if (socket->current_page->buffer()->pos() > 0) {
        socket->listen_socket->add_page_entry(socket->current_page, socket->current_page->buffer()->pos(),
                                            socket);
      }
      socket->current_page->dec_refcount();
      socket->current_page = nullptr;
  }

  socket->close();
}


void
netio::posix_process_incoming_data(netio::posix_recv_socket* socket)
{
    size_t bytes_consumable;
    ssize_t count;

    DEBUG_LOG("Processing incoming data in recv socket 0x%x", socket);

    while (1)
    {
        if(socket->current_page == nullptr)
        {
            DEBUG_LOG("Fetching page");
            bool page_available = socket->try_fetch_page(&(socket->current_page));
            if(page_available)
            {
                DEBUG_LOG("page available");
                socket->current_page->buffer()->reset();

                /* We need to "own" the page while it is being processed otherwise
                 * the creation (refcount = 1) and release (refcount = 0) of a
                 * reference on this buffer would release this buffer (reset it and
                 * put it back in the buffer_feeder queue). This happens when
                 * deserializing messages with the reference_deserializer: a
                 * netio::message::fragment is created which increment the refcount
                 * (it references data within this buffer). Once the message is done
                 * being processed, it is destroyed and the refcount is decreased:
                 * if the refcount is not increased here, this would be the last
                 * reference and the buffer will be release()'d. If there is another
                 * message in this buffer, this is just wrong.
                 */
                socket->current_page->inc_refcount();
                bytes_consumable = socket->current_page->buffer()->available();
            }
            else
            {
                DEBUG_LOG("No page available");
                return;
            }
        }
        else
        {
            bytes_consumable = socket->current_page->buffer()->available();
            DEBUG_LOG("Reusing old page, bytes available %d", bytes_consumable);
        }

        count = read (socket->ctx.fd, socket->current_page->buffer()->end(),
                      bytes_consumable);
        DEBUG_LOG("result of read: count=%d", count);

        if (count == -1)
        {
            /* When errno == EAGAIN it's just that we have read all available data */
            int errsv=errno;
            if (errsv == EBADF) {
                DEBUG_LOG("socket closed while reading");
                posix_end_processing_and_close_socket(socket);
                return;
            } else if (errsv != EAGAIN) {
                //raise_errno_exception();
                DEBUG_LOG("socket errno %s. Closing socket.", strerror(errsv));
                posix_end_processing_and_close_socket(socket);
            }
            break;
        }
        else if (count == 0)
        {
            /* End of file. The remote has closed the
               connection. The destructor will close the connection. */
            DEBUG_LOG("end-of-file: closing socket");
            posix_end_processing_and_close_socket(socket);
            return;
        }
        else
        {
            socket->current_page->buffer()->advance(count);
            if (socket->current_page->buffer()->available() == 0)
            {
                socket->listen_socket->add_page_entry(socket->current_page, socket->pagesize(), socket);

                /* This does not necessarily release the buffer: if there was
                 * a partial message in this page, a fragment created by the
                 * deserializer still holds a reference to this buffer. It is
                 * waiting for the next buffers to complete the message.
                 */
                socket->current_page->dec_refcount();
                socket->current_page = nullptr;
            }
        }
    }

    if(socket->current_page != nullptr && socket->current_page->buffer()->pos() > 0)
    {
        socket->listen_socket->add_page_entry(socket->current_page, socket->current_page->buffer()->pos(),
                                              socket);
        // See comment above
        socket->current_page->dec_refcount();
        socket->current_page = nullptr;
    }
}

netio::posix_recv_socket::posix_recv_socket(event_loop* evloop, backend_listen_socket* ls, int fd)
    : backend_recv_socket(evloop, ls), endpoint_is_cached(false)
{
    current_page = nullptr;

    ctx.fd = fd;
    ctx.data = this;
    ctx.fn = [](int sfd, void* data)
    {
        posix_recv_socket* socket = (posix_recv_socket*)data;
        posix_process_incoming_data(socket);
    };
    evloop->register_read_fd_et(&ctx);

    feeder.register_buf_available_cb([](void* data)
    {
        posix_recv_socket* socket = (posix_recv_socket*)data;
        posix_process_incoming_data(socket);
    }, this);
}


netio::endpoint
netio::posix_recv_socket::remote_endpoint()
{
    if(!endpoint_is_cached)
    {
        socklen_t local_socket_addr_len = sizeof(struct sockaddr_storage);
        DEBUG_LOG("Storage available: %d", local_socket_addr_len);
        if(-1 == getpeername(this->ctx.fd, cached_endpoint.sockaddr(), &local_socket_addr_len))
        {
            //netio::raise_errno_exception();
            //Exception handling, here and in backend_recv_socket::process_message
            int errsv = errno;
            INFO_LOG("Handling getpeername error %s", strerror(errsv));
            cached_endpoint.load_sockaddr("0", 0);
            INFO_LOG("The endpoint IP address has been set to %s port %d", cached_endpoint.address().c_str(), cached_endpoint.port());
        }
        if(cached_endpoint.sockaddr()->sa_family == AF_INET)
        {
            DEBUG_LOG("peername: IPv4");
        }
        else
        {
            DEBUG_LOG("peername: IPv6");
        }
        std::string a = cached_endpoint.address();
        DEBUG_LOG("Getting peer name: '%s' socklen=%d", a.c_str(), local_socket_addr_len);
        endpoint_is_cached = true;
    }
    return cached_endpoint;
}


netio::posix_recv_socket::~posix_recv_socket()
{
    DEBUG_LOG("Closing recv socket 0x%x, FD %d", this, ctx.fd);
    if(current_page)
        current_page->release();
    evloop->unregister_fd(&ctx);
    ::close(ctx.fd); //this should not be closed again at eventloop.cpp: 99 with EPOLLRDHUP!
}


netio::backend_send_socket*
netio::posix_backend::make_send_socket(event_loop* evloop, sockcfg cfg)
{
    return new posix_send_socket(evloop, cfg);
}


netio::backend_listen_socket*
netio::posix_backend::make_listen_socket(event_loop* evloop, endpoint ep, netio::context* c,
                                         sockcfg cfg)
{
    return new posix_listen_socket(evloop, ep, c, cfg);
}
