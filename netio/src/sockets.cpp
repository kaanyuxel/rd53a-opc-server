#include "netio/netio.hpp"
#include "backend.hpp"
#include "serialization.hpp"

#include <mutex>


//#define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[sockets@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif

// for subscription_message_header::type
static const uint8_t MSG_SUBSCRIBE      = 0;
static const uint8_t MSG_UNSUBSCRIBE    = 1;
static const uint8_t MSG_SUBSCRIBE_LL   = 2;

struct subscription_message_header
{
    uint8_t type;
    netio::tag tag;
    uint16_t port;
};


netio::socket::socket(context* ctx, sockcfg cfg)
{
    this->ctx = ctx;
    this->cfg = cfg;
}


netio::low_latency_send_socket::low_latency_send_socket(context* ctx, sockcfg cfg)
    : netio::socket(ctx, cfg)
{
    socket = ctx->backend()->make_send_socket(ctx->event_loop(), cfg);
}


netio::low_latency_send_socket::~low_latency_send_socket()
{
  if(socket) {
    delete socket;
    socket = nullptr;
  }
}


void
netio::low_latency_send_socket::connect(const endpoint& ep)
{
    peer_ = ep;
    socket->connect(ep);
}


void
netio::low_latency_send_socket::disconnect()
{
    DEBUG_LOG("Calling low_latency_send_socket::disconnect");
    //socket->disconnect(); already called by send_socket destructor
}


bool
netio::low_latency_send_socket::is_connecting() const
{
    DEBUG_LOG("Checking if ll socket is connecting");
    return socket->connection_state() == backend_send_socket::CONNECTING;
}


bool
netio::low_latency_send_socket::is_open() const
{
    DEBUG_LOG("Checking if ll socket is open");
    return socket->connection_state() == backend_send_socket::OPEN;
}


bool
netio::low_latency_send_socket::is_closed() const
{
    DEBUG_LOG("Checking if ll socket is closed");
    return socket->connection_state() == backend_send_socket::CLOSED;
}


netio::endpoint netio::low_latency_send_socket::peer() const
{
    return peer_;
}


void
netio::low_latency_send_socket::send(const message& msg)
{
    netio::msgheader header;
    header.len = msg.size();

    reusable_buffer* rb = new reusable_buffer(header.len + sizeof(header), NULL, ctx);
    buffer* buf = rb->buffer();

    buf->append((char*)&header, sizeof(header));

    for(const netio::message::fragment* p=msg.fragment_list(); p != nullptr; p = p->next)
    {
        buf->append((const char*)p->data[0], p->size[0]);
        if(p->data[1] != nullptr)
            buf->append((const char*)p->data[1], p->size[1]);
    }

    try {
        socket->send_buffer(rb);
    } catch(std::runtime_error& e) {
            // AGAIN
    }
}


netio::recv_socket::recv_socket(context* ctx, unsigned short port, sockcfg cfg)
    : netio::socket(ctx, cfg), connection_status(OPEN)
{
    socket = ctx->backend()->make_listen_socket(ctx->event_loop(), netio::endpoint("0.0.0.0", port),
                                                ctx, cfg);
    socket->listen();
}


netio::recv_socket::~recv_socket()
{
    close();
    if(socket != nullptr)
    {
        delete socket;
        socket = nullptr;
    }
}


void
netio::recv_socket::recv(message& msg)
{
    while(connection_status == OPEN)
    {
        if(socket->pop_message_entry(&msg) == true)
        {
            return;
        }
        else
        {
            socket->try_process_page();
        }
    }
    // if connection CLOSED
    throw netio::connection_closed();
}


void
netio::recv_socket::recv(endpoint& ep, message& msg)
{
    while(connection_status == OPEN)
    {
        if(socket->pop_message_entry(&msg, &ep) == true)
        {
            return;
        }
        else
        {
            socket->try_process_page();
        }
    }
    // if connection CLOSED
    throw netio::connection_closed();
}


void
netio::recv_socket::close()
{
    connection_status = CLOSED;
    //socket->close(); TODO close backend connection (ont only in desctructor)
}


netio::endpoint
netio::recv_socket::listen_endpoint() const
{
    return this->socket->endpoint();
}


netio::low_latency_recv_socket::low_latency_recv_socket(context* ctx, unsigned short port,
                                                        callback_fn fn, sockcfg cfg)
    : netio::socket(ctx, cfg), feeder(cfg.get(sockcfg::BUFFER_PAGES_PER_CONNECTION),
                                      cfg.get(sockcfg::PAGESIZE),
                                      ctx)
{
    socket = ctx->backend()->make_listen_socket(ctx->event_loop(), netio::endpoint("0.0.0.0", port),
                                                ctx, cfg);
    socket->register_cb_on_data_received([this]()
    {
        socket->process_page();
    });
    socket->register_cb_on_msg_received([fn, this]()
    {
        netio::message msg;
        endpoint ep;
        socket->pop_message_entry(&msg, &ep);
        fn(ep, msg);
    });

    socket->listen();
}


netio::low_latency_recv_socket::~low_latency_recv_socket()
{
  DEBUG_LOG("Destroying low_latency_recv_socket");
  if(socket != nullptr) {
    delete(socket);
    socket = nullptr;
  }
}


netio::endpoint
netio::low_latency_recv_socket::listen_endpoint() const
{
    return this->socket->endpoint();
}


static void
periodic_flush(void* data)
{
    DEBUG_LOG("periodic flush");
    netio::buffered_send_socket* socket = (netio::buffered_send_socket*)data;
    if(socket->is_open()) {
      socket->flush();
    } else {
      socket->disconnect(); // will close the FD and timer
    }
}


netio::buffered_send_socket::buffered_send_socket(context* ctx, sockcfg cfg)
    : netio::socket(ctx, cfg),
      tmr(ctx->event_loop(), periodic_flush, this),
      feeder(cfg.get(sockcfg::BUFFER_PAGES_PER_CONNECTION),
             cfg.get(sockcfg::PAGESIZE),
             ctx)
{
    socket = ctx->backend()->make_send_socket(ctx->event_loop(), cfg);
    while(feeder.try_pop(&current_buffer) == false)
    {
        std::this_thread::yield();
    }
}


netio::buffered_send_socket::~buffered_send_socket()
{
  DEBUG_LOG("Destroying buffered_send_socket");
  if(socket) {
    delete socket;
    socket = nullptr;
  }
}


void
netio::buffered_send_socket::connect(const endpoint& ep)
{
    DEBUG_LOG("buffered_send_socket connect");
    peer_ = ep;
    socket->connect(ep);
    tmr.start(cfg.get(sockcfg::FLUSH_INTERVAL_MILLISECS));
}


void
netio::buffered_send_socket::disconnect()
{
  DEBUG_LOG("buffered_send_socket disconnect");
    if(this->is_open()){
        tmr.stop();
        socket->disconnect();
    }
    else return;
}


void
netio::buffered_send_socket::send(const message& msg)
{
    DEBUG_LOG("buffered_send_socket::send");
    std::lock_guard<netio::spinlock> g(lock);
    DEBUG_LOG("buffered_send_socket::unlocked");

    if(socket->connection_state() == backend_send_socket::CLOSED) {
      return;
    }

    // Write message header
    size_t msg_size = msg.size();
    msgheader header;
    header.len = msg_size;
    int bytes_written;

    DEBUG_LOG("buffer: 0x%x   end: 0x%x   len: %d   bytes available: %d",
              current_buffer->buffer()->data(), current_buffer->buffer()->end(),
              current_buffer->buffer()->pos(), current_buffer->buffer()->available());

    DEBUG_LOG("serializing header (len=%d)", header.len);
    bytes_written = serialize_header_to_buffer(current_buffer->buffer()->end(),
                                               current_buffer->buffer()->available(),	header);
    while(bytes_written == 0)
    {
        non_mutex_flush();
        bytes_written = serialize_header_to_buffer(current_buffer->buffer()->end(),
                                                   current_buffer->buffer()->available(),	header);
    }
    current_buffer->buffer()->advance(bytes_written);
    DEBUG_LOG("wrote %d bytes (for header)", bytes_written);

    // Write message body
    for(const netio::message::fragment* p = msg.fragment_list(); p != nullptr; p = p->next)
    {
        unsigned int n = 0;
        while(n < p->size[0])
        {
            DEBUG_LOG("buffer: 0x%x   end: 0x%x   len: %d   bytes available: %d",
                      current_buffer->buffer()->data(), current_buffer->buffer()->end(),
                      current_buffer->buffer()->pos(), current_buffer->buffer()->available());
            size_t bytes_written = serialize_to_buffer(current_buffer->buffer()->end(),
                                                       current_buffer->buffer()->available(),
                                                       (const char*)p->data[0], p->size[0], &n);
            current_buffer->buffer()->advance(bytes_written);
            DEBUG_LOG("wrote %d bytes (for fragment)", bytes_written);
            if(bytes_written == 0)
            {
                non_mutex_flush();
            }
        }
        if(p->data[1] != nullptr)
        {
            n = 0;
            while(n < p->size[1])
            {
                DEBUG_LOG("buffer: 0x%x   end: 0x%x   len: %d   bytes available: %d",
                          current_buffer->buffer()->data(), current_buffer->buffer()->end(),
                          current_buffer->buffer()->pos(), current_buffer->buffer()->available());
                size_t bytes_written = serialize_to_buffer(current_buffer->buffer()->end(),
                                                           current_buffer->buffer()->available(),
                                                           (const char*)p->data[1], p->size[1], &n);
                current_buffer->buffer()->advance(bytes_written);
                DEBUG_LOG("wrote %d bytes (for fragment)", bytes_written);
                if(bytes_written == 0)
                {
                    non_mutex_flush();
                }
            }
        }
    }
    DEBUG_LOG("buffer: 0x%x   end: 0x%x   len: %d   bytes available: %d",
              current_buffer->buffer()->data(), current_buffer->buffer()->end(),
              current_buffer->buffer()->pos(), current_buffer->buffer()->available());
}


void
netio::buffered_send_socket::non_mutex_flush()
{
    if(current_buffer->buffer()->pos() > 0)
    {
        DEBUG_LOG("flush");
        try
        {
            socket->send_buffer(current_buffer);
            while(feeder.try_pop(&current_buffer) == false)
            {
              std::this_thread::yield();
            }
        }
        catch(std::runtime_error& e)
        {
            // AGAIN
        }
    }
}


void
netio::buffered_send_socket::flush()
{
    DEBUG_LOG("periodic flush");
    if(lock.try_lock())
    {
      if(current_buffer->buffer()->pos() > 0)
	{
	  DEBUG_LOG("flush (single try)");
	  try
	    {
	      DEBUG_LOG("single flush: sending buffer");
	      socket->send_buffer(current_buffer);
	      DEBUG_LOG("single flush: buffer sent, now getting a new buffer");
	      if(feeder.try_pop(&current_buffer) == false)
		{
		  DEBUG_LOG("single flush: no buffers available, yielding thread and then trying again");
		}
	    }
	  catch(std::exception& e)
	    {
	      // AGAIN
	      DEBUG_LOG(e.what());
	      //lock.unlock();
	    }
	  catch(...)
	    {
	      DEBUG_LOG("single flush: unknown exception");
	    }
	}
      DEBUG_LOG("unlocking");
      lock.unlock();
    }
}


void netio::buffered_send_socket::register_cb_on_connection_opened(std::function<void()> fn)
{
    socket->register_cb_on_connection_opened(fn);
}


void netio::buffered_send_socket::register_cb_on_connection_closed(std::function<void()> fn)
{
    socket->register_cb_on_connection_closed(fn);
}


bool netio::buffered_send_socket::is_connecting() const
{
    DEBUG_LOG("Checking if ht socket is connecting");
    if(socket){return socket->connection_state() == backend_send_socket::CONNECTING;}
    else return 0;
}


bool netio::buffered_send_socket::is_open() const
{
    DEBUG_LOG("Checking if ht socket is open");
    if(socket)return socket->connection_state() == backend_send_socket::OPEN;
    else return 0;
}


bool netio::buffered_send_socket::is_closed() const
{
    DEBUG_LOG("Checking if ht socket is closed");
    if(socket)return socket->connection_state() == backend_send_socket::CLOSED;
    else return 1;
}



netio::endpoint netio::buffered_send_socket::peer() const
{
    return peer_;
}


netio::tag_publisher::tag_publisher(tag t, publish_socket & pub_socket)
    : t(t)
    , pub_socket(pub_socket)
    , ht_sockets(pub_socket.subscriptions[t])
    , ll_sockets(pub_socket.subscriptions_ll[t])
{}


size_t
netio::tag_publisher::publish(const message& msg)
{
    DEBUG_LOG("Publish message using tag_publisher, tag=%d", t);
    //return pub_socket.publish(t, msg);

    //std::lock_guard<std::mutex> lock(pub_socket.subscription_mutex);

    size_t bytes_sent = 0;
    size_t const msgsize = msg.size();

    std::set<low_latency_send_socket*> closed_sockets_ll;
    auto itll = ll_sockets.begin();
    while(itll != ll_sockets.end())
    {
        low_latency_send_socket* socket = itll->get();
        if(socket->is_connecting())
        {
          DEBUG_LOG("ll connection is still connecting, ignoring for publish");
          ++itll;
          continue;
        }
        socket->send(msg);
        bytes_sent += msgsize;
        if(!socket->is_open())
        {
            closed_sockets_ll.insert(socket);
            bytes_sent -= msgsize;
        }
        ++itll;
    }
    if (!closed_sockets_ll.empty())
        pub_socket.clean_closed_socket(pub_socket.subscriptions_ll, pub_socket.connections_ll, closed_sockets_ll);

    std::set<buffered_send_socket*> closed_sockets;
    auto it = ht_sockets.begin();
    while(it != ht_sockets.end())
    {
        buffered_send_socket* socket = it->get();
        if(socket->is_connecting())
        {
          DEBUG_LOG("ht connection is still connecting, ignoring for publish");
          ++it;
          continue;
        }
        socket->send(msg);
        bytes_sent += msgsize;
        if(!socket->is_open())
        {
            closed_sockets.insert(socket);
            bytes_sent -= msgsize;
        }
        else
        {
            it++;
        }
    }
    if (!closed_sockets.empty())
        pub_socket.clean_closed_socket(pub_socket.subscriptions, pub_socket.connections, closed_sockets);

    return bytes_sent;
}


netio::publish_socket::publish_socket(context* ctx, unsigned short port, sockcfg cfg)
    : socket(ctx, cfg), subscription_socket(ctx, port, [this](endpoint& ep, message& m)
{
    parse_message(ep, m);
})
{}


netio::publish_socket::~publish_socket()
{
    for(auto & kv : connections) {
        if(kv.second->is_closed()){continue;}
        kv.second->disconnect();
    }
    for(auto & kv : connections_ll) {
        if(kv.second->is_closed()){continue;}
        kv.second->disconnect();
    }
}


void
netio::publish_socket::parse_message(const endpoint& remote_ep, const message& m)
{
    // TODO: do this without a data copy
    auto data = m.data_copy();
    void* d = (void*)data.data();
    subscription_message_header* header = (subscription_message_header*)d;
    endpoint ep(remote_ep.address(), header->port);
    switch(header->type)
    {
    case MSG_SUBSCRIBE:
        subscribe(header->tag, ep);
        break;
    case MSG_SUBSCRIBE_LL:
        subscribe_ll(header->tag, ep);
        break;
    case MSG_UNSUBSCRIBE:
        unsubscribe(header->tag, ep);
        break;
    }
}


void
netio::publish_socket::subscribe(tag tag, const endpoint& ep)
{
    DEBUG_LOG("subscribe request for tag %d", tag);

    std::shared_ptr<buffered_send_socket> socket;
    connection_map::iterator con = connections.find(ep);

    if(con == connections.end()) {
        socket = std::make_shared<buffered_send_socket>(ctx, cfg);
        DEBUG_LOG("connecting to remote %s:%d", ep.address(), ep.port());
        try {
            socket->connect(ep);
        }
        catch(...) {
            DEBUG_LOG("Could not connect to remote");
            return;
        }
        std::lock_guard<std::mutex> lock(subscription_mutex);
        auto res = connections.insert(connection_map::value_type(ep, socket));
        con = res.first;
    } else {
        socket = con->second;
    }

    {
      std::lock_guard<std::mutex> lock(subscription_mutex);
      auto& subs_for_tag = subscriptions[tag];
      subs_for_tag.emplace_back(socket);
    }

    DEBUG_LOG("calling callback");
    if(cb_subscribe)
        cb_subscribe(tag, ep);
}


void
netio::publish_socket::subscribe_ll(tag tag, const endpoint& ep)
{
    DEBUG_LOG("low-latency subscribe request for tag %lu", tag);

    std::shared_ptr<low_latency_send_socket> socket;
    connection_map_ll::iterator con = connections_ll.find(ep);

    if(con == connections_ll.end()) {
        socket = std::make_shared<low_latency_send_socket>(ctx, cfg);
        DEBUG_LOG("No previous connection present");
        try {
            socket->connect(ep);
        }
        catch(...) {
            DEBUG_LOG("Could not connect to remote");
            return;
        }
        DEBUG_LOG("LL SOCK adding %d socket peer %s %d ep %s %d", tag, socket->peer().address().c_str(), socket->peer().port(),  ep.address().c_str(), socket->peer().port());
        std::lock_guard<std::mutex> lock(subscription_mutex);
        auto res = connections_ll.insert(connection_map_ll::value_type(ep, socket));
        con = res.first;
    } else {
        DEBUG_LOG("LL SUB: Previous connection present for %lu", tag);
        socket = con->second;
    }

    {
      std::lock_guard<std::mutex> lock(subscription_mutex);
      auto& subs_for_tag = subscriptions_ll[tag];
      subs_for_tag.emplace_back(socket);

      //for(auto s : subscriptions_ll[tag]){
      //  DEBUG_LOG("Check: sockets in list for tag %lu: %p address %s", tag, s, ep.address());
      //}
    }

    DEBUG_LOG("calling callback");
    if(cb_subscribe)
        cb_subscribe(tag, ep);
}


void
netio::publish_socket::unsubscribe(tag tag, const endpoint& ep)
{
    auto& subsll = subscriptions_ll[tag];
    auto itll = subsll.begin();
    int count = 0;
    while (itll != subsll.end()) {
        low_latency_send_socket* socket = itll->get();
        DEBUG_LOG("LLSOCK: unsubscribing tag %lu ll socket %p counter %d", tag, socket, count);
        DEBUG_LOG("LLSOCK: peer ip %s %d ep ip %s %d", socket->peer().address().c_str(), socket->peer().port(),  ep.address().c_str(), socket->peer().port());
        if (socket && socket->peer() == ep) {
            std::lock_guard<std::mutex> lock(subscription_mutex);
            itll = subsll.erase(itll);
            DEBUG_LOG("LLSOCK: erased");
        } else {
            ++itll;
            DEBUG_LOG("LLSOCK: advancing iterator");
        }
        ++count;
    }

    auto& subs = subscriptions[tag];
    auto it = subs.begin();
    while (it != subs.end()) {
        DEBUG_LOG("unsubscribing ht subscription");
        buffered_send_socket* socket = it->get();
        if (socket && socket->peer() == ep) {
            std::lock_guard<std::mutex> lock(subscription_mutex);
            it = subs.erase(it);
        } else {
            ++it;
        }
    }

    // Close connections that no longer have active subscriptions
    for (connection_map::iterator itcon(connections.begin()); itcon != connections.end(); /* no inc */) {
        if (itcon->second.use_count() == 1) {
            // this reference is the last one <=> no subscription uses it
            itcon->second->disconnect();
            std::lock_guard<std::mutex> lock(subscription_mutex);
            itcon = connections.erase(itcon);
        } else {
            ++itcon;
        }
    }

    for (connection_map_ll::iterator itcon(connections_ll.begin()); itcon != connections_ll.end(); /* no inc */) {
        if (itcon->second.use_count() == 1) {
            // this reference is the last one <=> no subscription uses it
            itcon->second->disconnect();
            std::lock_guard<std::mutex> lock(subscription_mutex);
            itcon = connections_ll.erase(itcon);
        } else {
            ++itcon;
        }
    }

    if(cb_unsubscribe)
        cb_unsubscribe(tag, ep);
}


void
netio::publish_socket::register_subscribe_callback(std::function<void(tag, endpoint)> cb)
{
    cb_subscribe = cb;
}


void
netio::publish_socket::register_unsubscribe_callback(std::function<void(tag, endpoint)> cb)
{
    cb_unsubscribe = cb;
}


size_t
netio::publish_socket::publish(tag tag, const message& msg)
{
    size_t bytes_sent = 0;
    DEBUG_LOG("publishing message tag=%d", tag);
    bool ll_tag_found = false; bool ht_tag_found = false;

    std::list<std::shared_ptr<low_latency_send_socket>> subsll;
    try{
        subsll = subscriptions_ll.at(tag);
        ll_tag_found = true;
    }
    catch (const std::exception& e) {
        ll_tag_found = false;
    }

    if(ll_tag_found){
        std::set<low_latency_send_socket*> closed_sockets_ll;
        auto itll = subsll.begin();
        while (itll != subsll.end()) {
            DEBUG_LOG("publishing message to ll endpoint");
            low_latency_send_socket* socket = nullptr;
            try {
                socket = itll->get();
            }
            catch (const std::exception& e) {
                DEBUG_LOG("Invalid socket pointer from subscription list  %s\n", e.what());
                ++itll;
                continue;
            }

            if(socket->is_connecting())
            {
                DEBUG_LOG("ll connection is still connecting, ignoring for publish");
                ++itll;
                continue;
            }
            socket->send(msg);
            bytes_sent += msg.size();
            if(socket->is_closed())
            {
                closed_sockets_ll.insert(socket);
                bytes_sent -= msg.size();
            }
            ++itll;
        }

        if (!closed_sockets_ll.empty()){
            std::lock_guard<std::mutex> lock(subscription_mutex);
            clean_closed_socket(subscriptions_ll, connections_ll, closed_sockets_ll);
        }
    }

    std::list<std::shared_ptr<buffered_send_socket>> subs;
    try {
        subs = subscriptions.at(tag);
        ht_tag_found = true;
    }
    catch (const std::exception& e) {
        ht_tag_found = false;
    }

    if(ht_tag_found){
        std::set<buffered_send_socket*> closed_sockets;
        auto it = subs.begin();
        while (it != subs.end()) {
            DEBUG_LOG("publishing message to endpoint");
            buffered_send_socket* socket;
            try{
                socket = it->get();
            }
            catch (const std::exception& e) {
                DEBUG_LOG("Invalid socket pointer from subscription list  %s\n", e.what());
                ++it;
                continue;
            }
            if(!socket){continue;}
            if(socket->is_connecting())
            {
            DEBUG_LOG("ht connection is still connecting, ignoring for publish");
            ++it;
            continue;
            }
            socket->send(msg);
            bytes_sent += msg.size();
            if(socket->is_closed())
            {
                closed_sockets.insert(socket);
                bytes_sent -= msg.size();
            }
            ++it;
        }

        if (!closed_sockets.empty()){
            std::lock_guard<std::mutex> lock(subscription_mutex);
            clean_closed_socket(subscriptions, connections, closed_sockets);
        }
    }

    if (!(ht_tag_found || ll_tag_found)) {
      DEBUG_LOG("Cannot publish message for tag %lu. No subscription.", tag);
      return 0;
    }
    return bytes_sent;
}


netio::subscribe_socket::subscribe_socket(context* ctx, sockcfg cfg)
    : socket(ctx, cfg), receiving_socket(ctx, 0, cfg)
{
}


netio::subscribe_socket::~subscribe_socket()
{
}


static void
subscription_connection(netio::low_latency_send_socket* subscription_socket, netio::endpoint& ep)
{
  // TODO Would be better to do this asynchronously rather than wait
  subscription_socket->connect(ep);
  while(!subscription_socket->is_open())
      usleep(5); // TODO
}

static void
subscription_message(netio::low_latency_send_socket* subscription_socket, uint8_t type, netio::tag tag,
                     uint16_t port)
{
    subscription_message_header header;
    header.type = type;
    header.tag = tag;
    header.port = port;

    netio::message m((uint8_t*)&header, sizeof(header));
    subscription_socket->send(m);
}


void
netio::subscribe_socket::subscribe(tag tag, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    subscription_message(&subscription_socket,
                         MSG_SUBSCRIBE,
                         tag,
                         receiving_socket.listen_endpoint().port());
}


void
netio::subscribe_socket::subscribe(tag* tags, unsigned n, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);
    for(unsigned i=0; i<n; i++)
    {
        subscription_message(&subscription_socket,
                             MSG_SUBSCRIBE,
                             tags[i],
                             receiving_socket.listen_endpoint().port());
    }
}


void
netio::subscribe_socket::unsubscribe(tag tag, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    subscription_message(&subscription_socket,
                         MSG_UNSUBSCRIBE,
                         tag,
                         receiving_socket.listen_endpoint().port());
}


void
netio::subscribe_socket::unsubscribe(tag* tags, unsigned n, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    for(unsigned i=0; i<n; i++)
    {
        subscription_message(&subscription_socket,
                             MSG_UNSUBSCRIBE,
                             tags[i],
                             receiving_socket.listen_endpoint().port());
    }
}


void
netio::subscribe_socket::recv(message& msg)
{
    receiving_socket.recv(msg);
}


void
netio::subscribe_socket::recv(endpoint& ep, message& msg)
{
    receiving_socket.recv(ep, msg);
}


void
netio::subscribe_socket::close()
{
    receiving_socket.close();
}


netio::low_latency_subscribe_socket::low_latency_subscribe_socket(
    context* ctx, low_latency_recv_socket::callback_fn fn, sockcfg cfg)
    : socket(ctx, cfg), receiving_socket(ctx, 0, fn, cfg)
{
}


netio::low_latency_subscribe_socket::~low_latency_subscribe_socket()
{
}


void
netio::low_latency_subscribe_socket::subscribe(tag tag, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    subscription_message(&subscription_socket,
                         MSG_SUBSCRIBE_LL,
                         tag,
                         receiving_socket.listen_endpoint().port());
    subscription_socket.disconnect();
}


void
netio::low_latency_subscribe_socket::subscribe(tag* tags, unsigned n, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    for(unsigned i=0; i<n; i++)
    {
        subscription_message(&subscription_socket,
                             MSG_SUBSCRIBE_LL,
                             tags[i],
                             receiving_socket.listen_endpoint().port());
    }
    subscription_socket.disconnect();
}


void
netio::low_latency_subscribe_socket::unsubscribe(tag tag, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    subscription_message(&subscription_socket,
                         MSG_UNSUBSCRIBE,
                         tag,
                         receiving_socket.listen_endpoint().port());
    subscription_socket.disconnect();
}


void
netio::low_latency_subscribe_socket::unsubscribe(tag* tags, unsigned n, endpoint ep)
{
    netio::low_latency_send_socket subscription_socket(ctx);
    subscription_connection(&subscription_socket, ep);

    for(unsigned i=0; i<n; i++)
    {
        subscription_message(&subscription_socket,
                             MSG_UNSUBSCRIBE,
                             tags[i],
                             receiving_socket.listen_endpoint().port());
    }
    subscription_socket.disconnect();
}
