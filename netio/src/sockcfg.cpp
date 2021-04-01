#include "netio/netio.hpp"

using namespace netio;

sockcfg::sockcfg()
{
    buffer_pages_per_connection = 128;
    flush_interval_millisecs = 2000;
    zero_copy = false;
    pagesize = 1048576;
    callback = nullptr;
    callback_data = nullptr;
}


sockcfg sockcfg::cfg()
{
    return sockcfg();
}


sockcfg&
sockcfg::operator()(sockcfg::cfgtype_t flag)
{
    (*this)(flag, 1);
    return *this;
}


sockcfg&
sockcfg::operator()(sockcfg::cfgtype_t key, uint64_t value)
{
    switch(key)
    {
    case BUFFER_PAGES_PER_CONNECTION:
        buffer_pages_per_connection = value;
        break;
    case FLUSH_INTERVAL_MILLISECS:
        flush_interval_millisecs = value;
        break;
    case PAGESIZE:
        pagesize = value;
        break;
    case ZERO_COPY:
        zero_copy = (bool)value;
        break;
    case CALLBACK:
        callback = (msg_rcvd_cb_t)value;
        break;
    case CALLBACK_DATA:
        callback_data = (void*)value;
        break;
    default:
        throw std::runtime_error("Key error in config value lookup");
    }
    return *this;
}


uint64_t
sockcfg::get(sockcfg::cfgtype_t key) const
{
    switch(key)
    {
    case BUFFER_PAGES_PER_CONNECTION:
        return buffer_pages_per_connection;
    case FLUSH_INTERVAL_MILLISECS:
        return flush_interval_millisecs;
    case PAGESIZE:
        return pagesize;
    case ZERO_COPY:
        return zero_copy;
    case CALLBACK:
        return (uint64_t)callback;
    case CALLBACK_DATA:
        return (uint64_t)callback_data;
    default:
        throw std::runtime_error("Key error in config value lookup");
    }
}
