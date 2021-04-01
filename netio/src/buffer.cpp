#include "netio/netio.hpp"
#include "backend.hpp"

//#define TEST_IT
#ifdef TEST_IT
#include "debug.hpp"
# define DEBUG_LOG( ... ) do { printf("[buffer@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif

netio::buffer::buffer(context* ctx)
{
    size_ = 0;
    pos_ = 0;
    data_ = NULL;
    ctx = ctx;
}


netio::buffer::buffer(size_t n, context* ctx)
    : ctx(ctx), size_(n), pos_(0)
{
    alloc();
}


netio::buffer::buffer(const buffer& other)
{
    size_ = other.size_;
    pos_ = other.pos_;
    alloc();
    ctx = other.ctx;
    memcpy(data_, other.data_, other.pos_);
}


netio::buffer::buffer(buffer&& other)
{
    size_ = other.size_;
    pos_ = other.pos_;
    data_ = other.data_;
    ctx = other.ctx;
    other.size_ = 0;
    other.pos_ = 0;
    other.data_ = NULL;
}


netio::buffer&
netio::buffer::operator=(const buffer& other)
{
    if(this != &other)
    {
        cleanup();
        size_ = other.size_;
        pos_ = other.pos_;
        alloc();
        ctx = other.ctx;
        memcpy(data_, other.data_, other.pos_);
    }
    return *this;
}


netio::buffer&
netio::buffer::operator=(buffer&& other)
{
    if(this != &other)
    {
        cleanup();
        size_ = other.size_;
        pos_ = other.pos_;
        data_ = other.data_;
        ctx = other.ctx;
        other.size_ = 0;
        other.pos_ = 0;
        other.data_ = NULL;
    }
    return *this;
}


netio::buffer::~buffer()
{
    cleanup();
}


netio::backend_buffer*
netio::buffer::backend_buffer()
{
    return backend_buf;
}


void
netio::buffer::alloc()
{
    backend_buf = ctx->backend()->make_buffer(size_);
    data_ = (char*)backend_buf->buffer();
}


void
netio::buffer::cleanup()
{
    if(data_)
    {
        delete backend_buf;
        data_ = NULL;
    }
}


size_t
netio::buffer::pos() const
{
    return pos_;
}


size_t
netio::buffer::size() const
{
    return size_;
}


size_t
netio::buffer::available() const
{
    return size_ - pos_;
}


char*
netio::buffer::data() const
{
    return data_;
}


char*
netio::buffer::end() const
{
    return data_+pos_;
}

void
netio::buffer::advance(size_t s)
{
    if(s > available())
        throw std::runtime_error("not enough space available in buffer");
    pos_ += s;
}


void
netio::buffer::append(const char* d, size_t s)
{
    if(s > available())
        throw std::runtime_error("not enough space available in buffer");
    memcpy(data_+pos_, d, s);
    advance(s);
}


void
netio::buffer::reset()
{
    pos_ = 0;
}


netio::reusable_buffer::reusable_buffer(size_t n, buffer_feeder* feeder, context* ctx)
    :  feeder(feeder), buf(n, ctx), ctx(ctx), refcnt(0)
{}


netio::buffer*
netio::reusable_buffer::buffer()
{
    return &buf;
}


void
netio::reusable_buffer::detach()
{
    feeder = nullptr;
}


void
netio::reusable_buffer::release()
{
    DEBUG_LOG("reusable_buffer::release %p with buffer: (data: %p, pos: %lu, size: %lu, feeder_null: %s)",
            (void*)this, (void*)(buf.data()), buf.pos(), buf.size(),
            feeder ? "no":"yes");
#ifdef TEST_IT
    //dump_stacktrace("netio::reusable_buffer::release()");
#endif
    refcnt = 0;
    buf.reset();
    if(feeder)
    {
        feeder->release(this);
    }
    else
    {
        delete this;
    }
}


void
netio::reusable_buffer::inc_refcount()
{
#ifdef TEST_IT
    //dump_stacktrace("netio::reusable_buffer::inc_refcount()");
#endif
    DEBUG_LOG("netio::reusable_buffer::inc_refcount(): %u", refcnt);
    refcnt++;
}


void
netio::reusable_buffer::dec_refcount()
{
#ifdef TEST_IT
    //dump_stacktrace("netio::reusable_buffer::dec_refcount()");
#endif
    DEBUG_LOG("netio::reusable_buffer::dec_refcount(): %u", refcnt);
    if(refcnt == 0)
        return; // Already released
    refcnt--;
    if(refcnt == 0)
        release();
}


netio::buffer_feeder::buffer_feeder(unsigned num_buffers, size_t size, context* ctx)
{
    this->ctx = ctx;
    this->buffersize_ = size;
    this->max_buffers = num_buffers;
    for(unsigned i=0; i<num_buffers; i++)
    {
        allocate_buffer();
    }
}


netio::buffer_feeder::~buffer_feeder()
{
    // detach all pages
    for(auto& page : all_buffers)
    {
        page->detach();
    }

    // clear free pages
    while(!queue.empty())
    {
        netio::reusable_buffer* buf;
        if(queue.try_pop(buf))
        {
            buf->release();
        }
    }
}


void
netio::buffer_feeder::allocate_buffer()
{
    all_buffers.push_back(new reusable_buffer(this->buffersize_, this, ctx));
    queue.push(all_buffers.back());
}


bool
netio::buffer_feeder::try_pop(reusable_buffer** buffer)
{
    bool bufavailable = queue.try_pop(*buffer);
    if(bufavailable)
    {
        DEBUG_LOG("buffer_feeder::try_pop got reusable_buffer: %p with buffer: (data: %p, pos: %lu, size: %lu)",
                (void*)(*buffer), (void*)((*buffer)->buffer()->data()), (*buffer)->buffer()->pos(),
                (*buffer)->buffer()->size());
#ifdef TEST_IT
        //dump_stacktrace("netio::buffer_feeder::try_pop()");
#endif
        return true;
    }
    else
    {
        buffer = nullptr;
        return false;
    }
}


size_t
netio::buffer_feeder::buffersize() const
{
    return buffersize_;
}


size_t
netio::buffer_feeder::num_total_buffers() const
{
    return all_buffers.size();
}


size_t
netio::buffer_feeder::num_available_buffers() const
{
    return queue.unsafe_size();
}


void
netio::buffer_feeder::release(netio::reusable_buffer* b)
{
    queue.push(b);
    if(buf_available_signal)
        buf_available_signal->fire();
    DEBUG_LOG("buffer_feeder::release reusable buffer: %p, available/total: %lu/%lu",
            (void*)b, num_available_buffers(), num_total_buffers());
}


void
netio::buffer_feeder::register_buf_available_cb(buf_available_fn fn, void* data)
{
    buf_available_signal = std::make_unique<netio::signal>(ctx->event_loop(), fn, data);
}


void
netio::buffer_feeder::clear_all_pages()
{
    DEBUG_LOG("clear_all_pages()");
    queue.clear();
    for(auto b : all_buffers)
    {
        delete b;
    }
    all_buffers.clear();
}
