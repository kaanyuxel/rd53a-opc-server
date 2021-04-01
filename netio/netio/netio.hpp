#ifndef NETIO_HPP
#define NETIO_HPP

#include <atomic>
#include <set>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include <queue>
#include <list>
#include <exception>

#include <netinet/in.h>

#include "tbb/concurrent_queue.h"


namespace netio
{

class backend;
class backend_listen_socket;
class backend_recv_socket;
class backend_send_socket;
class backend_buffer;
class reusable_buffer;


class spinlock
{
public:
    inline void lock()
    {
        while (lock_.test_and_set(std::memory_order_acquire))  // acquire lock
            ; // spin
    }

    inline bool try_lock()
    {
        return !lock_.test_and_set(std::memory_order_acquire);
    }

    inline void unlock()
    {
        lock_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;

};

/**
 * A central NetIO event loop.
 */
class event_loop
{
public:
    struct context
    {
        int fd;
        void* data;
        std::function<void(int,void*)> fn;
    };

    /**
     * Constructor
     */
    event_loop();

    /**
     * Destructor
     */
    ~event_loop();

    /**
     * run_forevermake h
     */
    void run_forever();
    void run_for(unsigned long long millisecs);
    void run_one();
    void stop();

    void register_read_fd(context* ctx);
    void register_write_fd(context* ctx);
    void register_read_fd_et(context* ctx);
    void register_write_fd_et(context* ctx);
    void unregister_fd(context* ctx);

    bool is_running() const;

private:
    int epollfd;
    spinlock lock;
    std::atomic_bool running;

    unsigned wait_for_events(int epollfd, unsigned timeout_millisecs);

    std::set<context*> unregistered_contexts;
};


class timer
{
public:
    timer(event_loop* evloop, std::function<void(void*)> fn, void* data);
    ~timer();

    void start(unsigned millisecs);
    void stop();

private:
    event_loop* evloop;
    event_loop::context ctx;
    std::function<void(void*)> callback_fn;
};


class signal
{
public:
    signal(event_loop* evloop, std::function<void(void*)> fn, void* data);
    ~signal();
    void fire();

private:
    event_loop* evloop;
    event_loop::context ctx;
    std::function<void(void*)> fn;
};


class context
{
public:
    context(std::string name);
    ~context();

    netio::event_loop* event_loop();
    netio::backend* backend();

private:
    netio::event_loop evloop;
    netio::backend* backend_;
};

class message
{
public:
    typedef void (*cleanup_fn)(message&);

    struct fragment
    {
        const uint8_t* data[2];
        size_t size[2];
        reusable_buffer* buffer[2];
        fragment* next;
    };

    message(cleanup_fn hook=NULL);
    message(const uint8_t* d, size_t size, reusable_buffer* rb=NULL, cleanup_fn hook=NULL);
    message(const std::vector<const uint8_t*>& data, const std::vector<size_t>& sizes,
            cleanup_fn hook=NULL);
    message(const std::vector<const uint8_t*>& data, const std::vector<size_t>& sizes,
            const std::vector<reusable_buffer*>& rbs, cleanup_fn hook=NULL);
    message(const uint8_t* const* data, const size_t* sizes, const size_t n, reusable_buffer** rbs=NULL,
            cleanup_fn hook=NULL);
    message(const std::string& str, reusable_buffer* rb=NULL);
    message(const message& msg);
    message(message&& other);
    ~message();
    message& operator= (const message& other);
    message& operator= (message&& other);

    uint8_t operator[](size_t i) const;

    size_t size(void) const;
    std::vector<uint8_t> data_copy(void) const;
    void serialize_to_usr_buffer(void* buf);

    void add_fragment(const uint8_t* data, size_t size, reusable_buffer* rb=NULL);
    const fragment* fragment_list() const;
    size_t num_fragments() const;

private:
    fragment head;

    cleanup_fn cleanup_hook;

    void clean_fragment(netio::message::fragment* f);
    void copy_data_to_buffer(uint8_t* buffer) const;
};


void message_cleanup_delete_data(message& m);

class endpoint
{
    struct sockaddr_storage addr;

public:
    endpoint(unsigned short port=12345);
    endpoint(std::string hostname, unsigned short port=12345);
    endpoint(const char* hostname, unsigned short port=12345);
    endpoint(const endpoint& other);

    void load_sockaddr(const char* hostname, unsigned short port);

    bool operator==(const endpoint& other) const;
    bool operator<(const endpoint& other) const;

    const struct sockaddr* sockaddr() const;
    struct sockaddr* sockaddr();

    std::string address() const;
    unsigned short port() const;
};

typedef void (*msg_rcvd_cb_t)(uint8_t*, size_t, void*);

class sockcfg
{
public:

    enum cfgtype_t
    {

        /* Max. number of pages per connectin that are buffered. */
        BUFFER_PAGES_PER_CONNECTION,

        /* Timeout in milleseconds before a flush in buffered sockets
           is triggered. */
        FLUSH_INTERVAL_MILLISECS,

        /* Size of pages. Needs to be set consistently on receiving and
           sending end of a communication pair. */
        PAGESIZE,

        /* Enables zero-copy mode. */
        ZERO_COPY,

        /* Use callback instead of message queue */
        CALLBACK,

        /* User data for callback */
        CALLBACK_DATA
    };


    static sockcfg cfg();
    sockcfg& operator()(cfgtype_t);
    sockcfg& operator()(cfgtype_t, uint64_t);

    uint64_t get(cfgtype_t) const;

private:
    sockcfg();

    uint32_t buffer_pages_per_connection;
    uint32_t flush_interval_millisecs;
    size_t pagesize;
    uint32_t recv_msg_queue_size;
    bool zero_copy;
    msg_rcvd_cb_t callback;
    void* callback_data;

    friend class socket;
};


class socket
{
public:
    socket(context* ctx, sockcfg cfg = sockcfg::cfg());

protected:
    context* ctx;
    sockcfg cfg;

    netio::backend* backend();
    netio::event_loop* event_loop();
};


class low_latency_send_socket : public socket
{
public:
    low_latency_send_socket(context* ctx, sockcfg cfg = sockcfg::cfg());
    virtual ~low_latency_send_socket();

    void connect(const endpoint& ep);
    void disconnect();
    void send(const message& msg);

    bool is_connecting() const;
    bool is_open() const;
    bool is_closed() const;
    netio::endpoint peer() const;

private:
    backend_send_socket* socket;
    netio::endpoint peer_;
};


class buffer
{
public:
    buffer(context* ctx);
    buffer(size_t n, context* ctx);
    buffer(const buffer& other);
    buffer(buffer&& other);
    buffer& operator=(const buffer& other);
    buffer& operator=(buffer&& other);
    virtual ~buffer();

    size_t pos() const;
    size_t size() const;
    size_t available() const;
    char* data() const;
    char* end() const;
    void append(const char* d, size_t s);
    void advance(size_t s);
    void reset();

    netio::backend_buffer* backend_buffer();

private:
    void alloc();
    void cleanup();

    context* ctx;
    netio::backend_buffer* backend_buf;
    char* data_;
    size_t size_;
    size_t pos_;
};


class buffer_feeder;

class reusable_buffer
{
    buffer_feeder* feeder;
    netio::buffer buf;
    context* ctx;
    unsigned refcnt;

public:
    reusable_buffer(size_t n, buffer_feeder* feeder, context* ctx);
    netio::buffer* buffer();
    void detach();
    void release();
    void inc_refcount();
    void dec_refcount();
};


class buffer_feeder
{
    std::list<reusable_buffer*> all_buffers;
    tbb::concurrent_queue<reusable_buffer*> queue;
    size_t buffersize_;
    unsigned max_buffers;
    context* ctx;

    std::unique_ptr<signal> buf_available_signal;

    void allocate_buffer();

public:
    buffer_feeder(unsigned num_buffers, size_t size, context* ctx);
    ~buffer_feeder();
    bool try_pop(reusable_buffer** buffer);
    size_t buffersize() const;
    size_t num_total_buffers() const;
    size_t num_available_buffers() const;
    void release(reusable_buffer* b);
    void clear_all_pages();

    typedef void(*buf_available_fn)(void*);
    void register_buf_available_cb(buf_available_fn fn, void* data );
};


class buffered_send_socket : public socket
{
public:
    buffered_send_socket(context* ctx, sockcfg cfg = sockcfg::cfg());
    ~buffered_send_socket();

    void connect(const endpoint& ep);
    void disconnect();
    void send(const message& msg);
    void flush();

    bool is_connecting() const;
    bool is_open() const;
    bool is_closed() const;

    netio::endpoint peer() const;

    void register_cb_on_connection_opened(std::function<void()> fn);
    void register_cb_on_connection_closed(std::function<void()> fn);
private:
    void non_mutex_flush();

    static const unsigned QUEUE_LENGTH = 64;

    netio::endpoint peer_;
    backend_send_socket* socket;
    timer tmr;
    spinlock lock;
    buffer_feeder feeder;
    reusable_buffer* current_buffer;
};


class recv_socket : public socket
{
public:
    recv_socket(context* ctx, unsigned short port, sockcfg cfg = sockcfg::cfg());
    virtual ~recv_socket();

    void recv(message& msg);
    void recv(endpoint& ep, message& msg);

    void close();

    endpoint listen_endpoint() const;

private:
    enum { OPEN, CLOSED } connection_status;

    backend_listen_socket* socket;
    std::function<void(recv_socket&)> cb_recv_msg;
};


class low_latency_recv_socket : public socket
{
public:
    typedef std::function<void(endpoint&, message&)> callback_fn;

    low_latency_recv_socket(context* ctx,
                            unsigned short port,
                            callback_fn fn,
                            sockcfg cfg = sockcfg::cfg());
    virtual ~low_latency_recv_socket();

    endpoint listen_endpoint() const;

private:
    backend_listen_socket* socket;
    buffer_feeder feeder;
};


typedef uint64_t tag;

typedef std::map<endpoint, std::shared_ptr<buffered_send_socket>>    connection_map;
typedef std::map<endpoint, std::shared_ptr<low_latency_send_socket>> connection_map_ll;
typedef std::map<tag, std::list<std::shared_ptr<buffered_send_socket>>>    subscription_map;
typedef std::map<tag, std::list<std::shared_ptr<low_latency_send_socket>>> subscription_map_ll;

class publish_socket; // Forward declaration

class tag_publisher
{
public:
    tag_publisher(tag t, publish_socket & pub_socket);
    size_t publish(const netio::message& msg);

private:
    tag t;
    publish_socket & pub_socket;
    std::list<std::shared_ptr<buffered_send_socket>> const & ht_sockets;
    std::list<std::shared_ptr<low_latency_send_socket>> const & ll_sockets;
};

class publish_socket : public socket
{
public:
    publish_socket(context* ctx,
                   unsigned short port,
                   sockcfg cfg = sockcfg::cfg());
    ~publish_socket();

    size_t publish(tag tag, const message& msg);
    void register_subscribe_callback(std::function<void(tag, endpoint)> cb);
    void register_unsubscribe_callback(std::function<void(tag, endpoint)> cb);

private:
    low_latency_recv_socket subscription_socket;

    connection_map connections;
    connection_map_ll connections_ll;
    subscription_map subscriptions;
    subscription_map_ll subscriptions_ll;
    std::mutex subscription_mutex;

    std::function<void(tag, endpoint)> cb_subscribe;
    std::function<void(tag, endpoint)> cb_unsubscribe;

    void parse_message(const endpoint& ep, const message& m);
    void subscribe(tag tag, const endpoint& ep);
    void subscribe_ll(tag tag, const endpoint& ep);
    void unsubscribe(tag tag, const endpoint& ep);

    // Do NOT call if you don't lock subscription_mutex
    template <typename TSubMap, typename TConnMap, typename TSocket>
    void clean_closed_socket(
              TSubMap & sub_map
            , TConnMap & conn_map
            , std::set<TSocket*> const & sockets);

    friend class tag_publisher;

public:

    std::unique_ptr<tag_publisher> get_tag_publisher(netio::tag tag)
    {
        return std::unique_ptr<tag_publisher>(new tag_publisher(tag, *this));
    }
};


template <typename TSubMap, typename TConnMap, typename TSocket>
void publish_socket::clean_closed_socket(
          TSubMap & sub_map
        , TConnMap & conn_map
        , std::set<TSocket*> const & sockets)
{
    for (auto itmap(sub_map.begin()); itmap != sub_map.end(); ++itmap) {
        typename std::list<std::shared_ptr<TSocket>> & sub_lst = itmap->second;
        for (auto itlist(sub_lst.begin()); itlist != sub_lst.end(); /*no inc*/) {
            typename std::set<TSocket*>::const_iterator itsocket = sockets.find(itlist->get());
            if (itsocket != sockets.end()) {
                itlist = sub_lst.erase(itlist);
            } else {
                ++itlist;
            }
        }
    }

    for (TSocket* socket: sockets) {
        conn_map.erase(socket->peer());
    }
}


class subscribe_socket : public socket
{
public:
    subscribe_socket(context* ctx,
                     sockcfg cfg = sockcfg::cfg());
    ~subscribe_socket();

    void subscribe(tag tag, endpoint ep);
    void subscribe(tag* tags, unsigned n, endpoint ep);
    void unsubscribe(tag tag, endpoint ep);
    void unsubscribe(tag* tags, unsigned n, endpoint ep);
    void recv(message& msg);
    void recv(endpoint& ep, message& msg);
    void close();

private:
    recv_socket receiving_socket;
};


class low_latency_subscribe_socket : public socket
{
public:
    low_latency_subscribe_socket(context* ctx,
                                 low_latency_recv_socket::callback_fn fn,
                                 sockcfg cfg = sockcfg::cfg());
    ~low_latency_subscribe_socket();

    void subscribe(tag tag, endpoint ep);
    void subscribe(tag* tags, unsigned n, endpoint ep);
    void unsubscribe(tag tag, endpoint ep);
    void unsubscribe(tag* tags, unsigned n, endpoint ep);

private:
    low_latency_recv_socket receiving_socket;
};


class exception : public std::exception
{
public:
    virtual char const * what() const noexcept
    {
        return "Generic NetIO error";
    }
};

class connection_closed : public netio::exception
{
public:
    virtual char const * what() const noexcept
    {
        return "Connection to remote was closed";
    }
};

}
#endif
