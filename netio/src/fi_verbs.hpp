#pragma once

#include "config.h"
#ifdef ENABLE_FIVERBS

#include "backend.hpp"

#include <rdma/fabric.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_errno.h>

namespace netio
{

void fi_verbs_process_send_socket_cq_event(int fd, void* ptr);
void fi_verbs_process_send_socket_cm_event(int fd, void* ptr);
void fi_verbs_process_listen_socket_cm_event(int fd, void* data);
void fi_verbs_process_recv_socket_cm_event(int fd, void* data);
void fi_verbs_process_recv_socket_cq_event(int fd, void* data);


class fi_verbs_send_socket : public backend_send_socket
{
public:
    fi_verbs_send_socket(event_loop* evloop, sockcfg cfg = sockcfg::cfg());
    virtual ~fi_verbs_send_socket();

    virtual void connect(const endpoint& ep);
    virtual void disconnect();
    virtual void send_buffer(netio::reusable_buffer* buffer);

private:
    struct fi_info* fi;
    struct fid_fabric *fabric;
    struct fid_eq *eq;
    struct fid_domain *domain;
    struct fid_ep *ep;
    struct fid_cq* cq;
    netio::event_loop::context ev_context;
    netio::event_loop::context ev_context_cm;
    uint64_t req_key = 0;
    std::atomic_ullong outstanding_completions;

    std::atomic_uint windowsize ;
    std::atomic_uint current_window;

    void handle_completion(fi_cq_msg_entry* entry);
    void handle_cq_error();
    int check_for_completions();
    void wait_for_completions(unsigned num);

    friend void fi_verbs_process_send_socket_cq_event(int fd, void* ptr);
    friend void fi_verbs_process_send_socket_cm_event(int fd, void* ptr);
};


class fi_verbs_listen_socket : public backend_listen_socket
{
public:
    fi_verbs_listen_socket(event_loop* evloop, netio::endpoint endpoint, netio::context* c,
                           sockcfg cfg = sockcfg::cfg());
    virtual ~fi_verbs_listen_socket();

    virtual void listen();
    virtual netio::endpoint endpoint() const;

private:
    struct fi_info* fi;
    struct fid_fabric *fabric;
    struct fid_eq *eq;
    struct fid_pep *pep;
    netio::event_loop::context ev_context;

    friend void fi_verbs_process_recv_socket_cq_event(int fd, void* data);
    friend void fi_verbs_process_listen_socket_cm_event(int fd, void* data);
};


class fi_verbs_recv_socket : public backend_recv_socket
{
public:
    fi_verbs_recv_socket(event_loop* evloop, backend_listen_socket* ls);
    virtual ~fi_verbs_recv_socket();

    netio::endpoint remote_endpoint();

private:
    struct fid_domain *domain;
    struct fid_ep *ep;
    struct fid_eq *eq;
    struct fid_cq* cq;
    uint64_t req_key;

    const unsigned BUFFER_PAGES = 32;

    void post_recv();

    netio::event_loop::context ev_context;
    netio::event_loop::context ev_context_cm;

    netio::endpoint cached_endpoint;
    bool endpoint_is_cached;

    friend void fi_verbs_process_listen_socket_cm_event(int fd, void* data);
    friend void fi_verbs_process_recv_socket_cm_event(int fd, void* data);
    friend void fi_verbs_process_recv_socket_cq_event(int fd, void* data);
};


class fi_verbs_buffer : public backend_buffer
{
public:
    fi_verbs_buffer(size_t s);
    virtual ~fi_verbs_buffer();
    struct fid_mr* mr;

protected:
    virtual void alloc();
};


class fi_verbs_backend : public backend
{
    backend_send_socket* make_send_socket(event_loop* evloop, sockcfg cfg = sockcfg::cfg());
    backend_listen_socket* make_listen_socket(event_loop* evloop, endpoint ep, netio::context* c,
                                              sockcfg cfg = sockcfg::cfg());
    backend_buffer* make_buffer(size_t size);
};

}


#endif
