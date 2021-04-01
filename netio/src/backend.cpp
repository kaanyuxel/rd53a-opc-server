#include "netio/netio.hpp"
#include "backend.hpp"
#include "utility.hpp"
#include "serialization.hpp"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>


//#define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[backend@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif


netio::backend_send_socket::backend_send_socket(event_loop* evloop, sockcfg cfg)
    : cfg(cfg)
{
    this->evloop = evloop;
    this->status = CLOSED;
}


netio::backend_send_socket::~backend_send_socket()
{
}


void
netio::backend_send_socket::register_cb_on_connection_opened(std::function<void()> fn)
{
    on_connection_opened = fn;
}


netio::backend_send_socket::state
netio::backend_send_socket::connection_state() const
{
    return status;
}


void
netio::backend_send_socket::register_cb_on_connection_closed(std::function<void()> fn)
{
    on_connection_closed = fn;
}


netio::backend_listen_socket::backend_listen_socket(event_loop* evloop, netio::endpoint ep,
                                                    netio::context* c, sockcfg cfg)
    : cfg(cfg)
{
    this->evloop = evloop;
    this->ep = ep;
    this->context = c;
}


netio::backend_listen_socket::~backend_listen_socket()
{
}


void
netio::backend_listen_socket::register_cb_on_connected(std::function<void(backend_recv_socket&)> fn)
{
    on_connected = fn;
}


void
netio::backend_listen_socket::register_cb_on_data_received(std::function<void(void)> fn)
{
    on_data_received = fn;
}


void
netio::backend_listen_socket::register_cb_on_msg_received(std::function<void(void)> fn)
{
    on_msg_received = fn;
}


void
netio::backend_listen_socket::add_page_entry(netio::reusable_buffer* page, size_t pos,
                                             backend_recv_socket* socket)
{
    DEBUG_LOG("add page of size %d", pos);
    msg_rcvd_cb_t callback = (msg_rcvd_cb_t)cfg.get(sockcfg::CALLBACK);
    if(callback != nullptr)
    {
        socket->process_page(page, pos);
    }
    else
    {
        page->inc_refcount();
        page_queue.push(page_entry(page, pos, socket));
        if(on_data_received) on_data_received();
    }
}


void
netio::backend_listen_socket::add_message_entry(netio::message& m, netio::endpoint ep)
{
    if (ep.port()!=0){
        message_queue.emplace(m, ep);
        if(on_msg_received) on_msg_received();
    }
    else{
        DEBUG_LOG("Caught null IP in backend_listen_socket::add_message_entry");
    }
}


bool
netio::backend_listen_socket::pop_message_entry(netio::message* message, netio::endpoint* ep)
{
    message_entry entry;
    if(message_queue.try_pop(entry))
    {
        *message = std::move(entry.msg);
        if(ep)
            *ep = entry.ep;
        return true;
    }

    return false;
}


void
netio::backend_listen_socket::process_page()
{
    page_entry entry;
    bool found_page = false;
    while(!found_page)
    {
        found_page = page_queue.try_pop(entry);
    }
    DEBUG_LOG("processing page of size %d", entry.size);
    entry.socket->process_page(entry.page, entry.size);
    entry.page->dec_refcount();
}


bool
netio::backend_listen_socket::try_process_page()
{
    page_entry entry;
    bool found_page = false;
    found_page = page_queue.try_pop(entry);
    if(found_page)
    {
        DEBUG_LOG("processing page of size %d", entry.size);
        entry.socket->process_page(entry.page, entry.size);
        entry.page->dec_refcount();
    }
    return found_page;
}


void callback_add_to_ls_msg_queue(netio::message& m, void* data)
{
    netio::backend_recv_socket* s = (netio::backend_recv_socket*)data;
    s->process_message(m);
}

netio::backend_recv_socket::backend_recv_socket(event_loop* evloop, backend_listen_socket* ls)
    : feeder(ls->cfg.get(sockcfg::BUFFER_PAGES_PER_CONNECTION), ls->cfg.get(sockcfg::PAGESIZE),
             ls->context)
    , active_pages(0)
{
    status = OPEN;
    this->evloop = evloop;
    this->listen_socket = ls;
    msg_rcvd_cb_t callback = (msg_rcvd_cb_t)ls->cfg.get(sockcfg::CALLBACK);
    if(callback != nullptr)
    {
        void* user_data = (void*)ls->cfg.get(sockcfg::CALLBACK_DATA);
        this->deserializer = std::unique_ptr<netio::new_deserializer>(new netio::callback_deserializer(
                                 callback, user_data));
    }
    else
    {
        if(ls->cfg.get(sockcfg::ZERO_COPY))
        {
            DEBUG_LOG("Using ZERO-COPY mode");
            this->deserializer = std::unique_ptr<netio::new_deserializer>(new netio::reference_deserializer());
        }
        else
        {
            this->deserializer = std::unique_ptr<netio::new_deserializer>(new netio::copy_deserializer());
        }
        this->deserializer->register_cb_on_msg_received(callback_add_to_ls_msg_queue, this);
    }
}


netio::backend_recv_socket::~backend_recv_socket()
{
}


void
netio::backend_recv_socket::process_page(netio::reusable_buffer* page, size_t size)
{
    deserializer->feed(page);
    active_pages--;
    release();
}


void
netio::backend_recv_socket::process_message(netio::message& msg)
{
    //DEBUG_LOG("Adding message of size %d to listen_sockets message queue", msg.size());
    listen_socket->add_message_entry(msg, this->remote_endpoint());
}


void
netio::backend_recv_socket::close()
{
    status = CLOSED;
    active_pages.store(0);
    release();
}


bool
netio::backend_recv_socket::try_fetch_page(netio::reusable_buffer** buffer)
{
    bool pageavailable = feeder.try_pop(buffer);
    if(pageavailable)
    {
        active_pages++;
    }
    return pageavailable;
}


size_t
netio::backend_recv_socket::pagesize()
{
    return feeder.buffersize();
}

void
netio::backend_recv_socket::release()
{
    DEBUG_LOG("release: status=%d  active_pages=%d", status, active_pages.load());
    if(status==CLOSED && active_pages==0)
        delete this;
}
