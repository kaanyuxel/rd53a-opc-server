#pragma once

#include "backend.hpp"

//Asynchronous FD writer or not?
//#define AFDW

namespace netio
{

#ifdef AFDW
class posix_send_socket;

class async_fd_writer {
public:
  async_fd_writer();
  async_fd_writer(int fd);
  async_fd_writer& operator=(const async_fd_writer& other);
  virtual ~async_fd_writer();

  void add_buffer(netio::reusable_buffer* buffer);
  void flush_all_blocking();
  void send();

private:
  bool connection_dead;
  std::mutex mtx;
  int fd;
  tbb::concurrent_queue<netio::reusable_buffer*> buffers_to_send;
  netio::reusable_buffer* current_buffer = NULL;
  size_t bytes_written, size;
};
#endif


class posix_send_socket : public backend_send_socket
{

#ifdef AFDW
    async_fd_writer fd_writer;
#endif

public:
    posix_send_socket(event_loop* evloop, sockcfg cfg = sockcfg::cfg());
    virtual ~posix_send_socket();

    virtual void connect(const endpoint& ep);
    virtual void disconnect();
    virtual void send_buffer(netio::reusable_buffer* buffer);
};


class posix_listen_socket : public backend_listen_socket
{
public:
    posix_listen_socket(event_loop* evloop, netio::endpoint endpoint, netio::context* c,
                        sockcfg cfg = sockcfg::cfg());
    virtual ~posix_listen_socket();

    virtual void listen();
    virtual netio::endpoint endpoint() const;

protected:
    void accept_connections();
};


class posix_recv_socket : public backend_recv_socket
{
public:
    posix_recv_socket(event_loop* evloop, backend_listen_socket* ls, int fd);
    virtual ~posix_recv_socket();

    netio::endpoint remote_endpoint();

    friend void posix_process_incoming_data(posix_recv_socket* socket);
    friend void posix_end_processing_and_close_socket(posix_recv_socket* socket);

private:
    netio::reusable_buffer* current_page;

    netio::endpoint cached_endpoint;
    bool endpoint_is_cached;
};


void posix_process_incoming_data(posix_recv_socket* socket);
// assumes that if socket->current_page is not null, the socket owns the page,
// i.e. refcount was incremented when fetched.
void posix_end_processing_and_close_socket(netio::posix_recv_socket* socket);

class posix_backend : public backend
{
    backend_send_socket* make_send_socket(event_loop* evloop, sockcfg cfg = sockcfg::cfg());
    backend_listen_socket* make_listen_socket(event_loop* evloop, endpoint ep, netio::context* c,
                                              sockcfg cfg = sockcfg::cfg());
};


}
