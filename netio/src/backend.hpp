#pragma once

#include "netio/netio.hpp"
#include "serialization.hpp"
#include "deserialization.hpp"
#include "tbb/concurrent_queue.h"


#define NETIO_INITIAL_PAGES (64)
#define NETIO_PAGESIZE (netio::buffered_send_socket::BUFFERSIZE)

namespace netio
{

class backend_send_socket
{
public:
    enum state { CONNECTING, OPEN, CLOSED };

    backend_send_socket(event_loop* evloop, sockcfg cfg = sockcfg::cfg());
    virtual ~backend_send_socket();

    virtual void connect(const endpoint& ep) = 0;
    virtual void disconnect() = 0;
    virtual void send_buffer(netio::reusable_buffer* buffer) = 0;

    state connection_state() const;

    void register_cb_on_connection_opened(std::function<void()> fn);
    void register_cb_on_connection_closed(std::function<void()> fn);

protected:
    event_loop* evloop;
    event_loop::context ctx;
    sockcfg cfg;

    state status;

    std::function<void()> on_connection_opened;
    std::function<void()> on_connection_closed;
};


class backend_listen_socket; // Forward declaration

class backend_recv_socket
{
public:
    backend_recv_socket(event_loop* evloop, backend_listen_socket* ls);
    virtual ~backend_recv_socket();

    virtual netio::endpoint remote_endpoint() = 0;
    void process_page(netio::reusable_buffer* page, size_t size);
    void process_message(netio::message& msg);

protected:
    event_loop* evloop;
    event_loop::context ctx;
    backend_listen_socket* listen_socket;

    // TODO: This introduces, via buffer_feeder, a dependency on netio::context
    // This should be redesigned to have a clean separation between user sockets and backend sockets
    netio::buffer_feeder feeder;

    std::unique_ptr<netio::new_deserializer> deserializer;
    std::atomic_int active_pages;
    enum connection_status
    {
        OPEN, CLOSED
    } status;

    void close();
    bool try_fetch_page(netio::reusable_buffer** buffer);
    size_t pagesize();
    void release();
};


class backend_listen_socket
{
public:
    backend_listen_socket(event_loop* evloop, endpoint endpoint, netio::context* context,
                          sockcfg cfg = sockcfg::cfg());
    virtual ~backend_listen_socket();

    virtual void listen() = 0;
    virtual netio::endpoint endpoint() const = 0;

    void register_cb_on_connected(std::function<void(backend_recv_socket&)> fn);
    void register_cb_on_data_received(std::function<void(void)> fn);
    void register_cb_on_msg_received(std::function<void(void)> fn);

    void add_page_entry(netio::reusable_buffer* page, size_t size, backend_recv_socket* socket);
    void add_message_entry(netio::message&, netio::endpoint ep);
    bool pop_message_entry(netio::message*, netio::endpoint* ep=NULL);
    void process_page();
    bool try_process_page();

protected:
    netio::context* context;
    event_loop* evloop;
    event_loop::context ctx;
    netio::endpoint ep;
    sockcfg cfg;

    struct message_entry
    {
        message_entry() {}
        message_entry(netio::message& m, netio::endpoint ep)
            : msg(std::move(m)), ep(ep) {}

        netio::message msg;
        netio::endpoint ep;
    };

    struct page_entry
    {
        page_entry(netio::reusable_buffer* page=NULL, size_t size=0, backend_recv_socket* socket=NULL)
            : page(page), size(size), socket(socket) {}
        netio::reusable_buffer* page;
        size_t size;
        backend_recv_socket* socket;
    };

    tbb::concurrent_queue<message_entry> message_queue;
    tbb::concurrent_bounded_queue<page_entry> page_queue;

    std::function<void(backend_recv_socket&)> on_connected;
    std::function<void(void)> on_data_received;
    std::function<void(void)> on_msg_received;

    friend class backend_recv_socket;
};


class backend_buffer
{
public:
    backend_buffer(size_t s)
    {
        size_ = s;
        this->alloc();
    }

    virtual ~backend_buffer()
    {
      delete[](ptr);
    }

    uint8_t* buffer()
    {
        return ptr;
    }
    size_t size()
    {
        return size_;
    }

protected:
    size_t size_;
    uint8_t* ptr;

    virtual void alloc()
    {
        ptr = new uint8_t[size_];
    }
};


class backend
{
public:
    virtual ~backend() {}
    virtual backend_send_socket* make_send_socket(event_loop* evloop, sockcfg cfg) = 0;
    virtual backend_listen_socket* make_listen_socket(event_loop* evloop, endpoint ep, netio::context*,
                                                      sockcfg cfg) = 0;
    virtual backend_buffer* make_buffer(size_t size)
    {
        return new backend_buffer(size);
    }
};


}
