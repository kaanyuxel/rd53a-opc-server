#include "netio/netio.hpp"
#include "backend.hpp"
#include "posix.hpp"
#include "fi_verbs.hpp"
#include "config.h"

netio::context::context(std::string name)
{
    backend_ = nullptr;
    if(name == "posix")
    {
        backend_ = new netio::posix_backend();
    }
#ifdef ENABLE_FIVERBS
    if(name == "fi_verbs")
    {
        backend_ = new netio::fi_verbs_backend();
    }
#endif
}

netio::context::~context()
{
    if(backend_)
        delete backend_;
}


netio::event_loop*
netio::context::event_loop()
{
    return &(this->evloop);
}


netio::backend*
netio::context::backend()
{
    return this->backend_;
}
