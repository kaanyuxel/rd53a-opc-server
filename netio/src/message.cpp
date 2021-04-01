#include "netio/netio.hpp"

#include <algorithm>

// #define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[message@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif


namespace netio
{


message::message(cleanup_fn hook) : cleanup_hook(hook)
{
    DEBUG_LOG("message()");
    head.data[0] = head.data[1] = nullptr;
    head.size[0] = head.size[1] = 0;
    head.buffer[0] = head.buffer[1] = nullptr;
    head.next = nullptr;
}


message::message(const uint8_t* d, size_t size, reusable_buffer* rb, cleanup_fn hook)
    : cleanup_hook(hook)
{
    DEBUG_LOG("message(uint8_t*, size_t)");

    head.data[0] = d;
    head.size[0] = size;
    head.buffer[0] = rb;

    head.data[1] = nullptr;
    head.size[1] = 0;
    head.buffer[1] = nullptr;

    head.next = nullptr;
}


message::message(const std::vector<const uint8_t*>& data,
                 const std::vector<size_t>& sizes,
                 cleanup_fn hook)
    : cleanup_hook(hook)
{
    DEBUG_LOG("message(stdvec<uint8_t*>, stdvec<size_t>)");
    fragment* p = &head;
    for(unsigned i=0; i<data.size(); )
    {
        p->data[0] = data[i];
        p->size[0] = sizes[i];
        p->buffer[0] = p->buffer[1] = nullptr;
        ++i;

        if(i < data.size()-1)
        {
            p->data[1] = data[i];
            p->size[1] = sizes[i];
            p->next = new fragment;
        }
        else if(data.size()==i)
        {
            p->data[1] = nullptr;
            p->size[1] = 0;
            p->next = nullptr;
        }
        else
        {
            p->data[1] = data[i];
            p->size[1] = sizes[i];
            p->next = nullptr;
        }
        p = p->next;
        ++i;
    }
}


message::message(const std::vector<const uint8_t*>& data,
                 const std::vector<size_t>& sizes,
                 const std::vector<reusable_buffer*>& rbs,
                 cleanup_fn hook)
    : message(data, sizes, hook)
{
    DEBUG_LOG("message(stdvec<uint8_t*>, stdvec<size_t>, stdvec<reusable_buffer*>)");

    fragment* p = &head;
    for(unsigned i=0; i<rbs.size(); )
    {
        DEBUG_LOG("setting buffer %i of msg 0x%x to 0x%x", i, this, rbs[i]);
        p->buffer[0] = rbs[i];
        ++i;

        if(i<rbs.size())
        {
            DEBUG_LOG("setting buffer %i of msg 0x%x to 0x%x", i, this, rbs[i]);
            p->buffer[1] = rbs[i];
        }
        p = p->next;
        ++i;
    }
}


message::message(const uint8_t* const* data,
                 const size_t* sizes,
                 const size_t n,
                 reusable_buffer** rbs,
                 cleanup_fn hook)
    : cleanup_hook(hook)
{
    DEBUG_LOG("message(uint8_t**, size_t*)");
    fragment* p = &head;
    for(unsigned i=0; i<n; )
    {
        p->data[0] = data[i];
        p->size[0] = sizes[i];
        if(rbs)
        {
            p->buffer[0] = rbs[i];
        }
        ++i;

        if(i<n-1)
        {
            p->data[1] = data[i];
            p->size[1] = sizes[i];
            if(rbs)
            {
                p->buffer[1] = rbs[i];
            }
            p->next = new fragment;
        }
        else if(n==i)
        {
            p->data[1] = nullptr;
            p->size[1] = 0;
            p->buffer[1] = nullptr;
            p->next = nullptr;
        }
        else
        {
            p->data[1] = data[i];
            p->size[1] = sizes[i];
            if(rbs)
            {
                p->buffer[1] = rbs[i];
            }
            p->next = nullptr;
        }
        p = p->next;
        ++i;
    }
}


message::message(const std::string& str, reusable_buffer* rb)
    : cleanup_hook(message_cleanup_delete_data)
{
    DEBUG_LOG("message(string)");

    head.data[0] = new uint8_t[str.size()];
    memcpy((void*)head.data[0], (void*)str.c_str(), str.size());
    head.size[0] = str.length();
    head.buffer[0] = rb;

    head.data[1] = nullptr;
    head.size[1] = 0;
    head.buffer[1] = nullptr;

    head.next = nullptr;
}


void temp_null_cleanup(netio::message&) {}


message::message(const message& msg)
{
    DEBUG_LOG("message(const message&)");
    cleanup_hook = msg.cleanup_hook;
    const fragment* p_other = &(msg.head);
    fragment* p_this = &head;
    while(p_this != nullptr)
    {
        p_this->data[0] = p_other->data[0];
        p_this->size[0] = p_other->size[0];
        p_this->buffer[0] = p_other->buffer[0];

        p_this->data[1] = p_other->data[1];
        p_this->size[1] = p_other->size[1];
        p_this->buffer[1] = p_other->buffer[1];

        if(p_other->next != nullptr)
            p_this->next = new fragment;
        else
            p_this->next = nullptr;
        p_this = p_this->next;
        p_other = p_other->next;
    }
}


message::message(message&& other)
    : cleanup_hook(other.cleanup_hook)
{
    DEBUG_LOG("message(messagse&&) (this=0x%x)", this);

    head.size[0] = other.head.size[0];
    head.data[0] = other.head.data[0];
    head.buffer[0] = other.head.buffer[0];

    head.size[1] = other.head.size[1];
    head.data[1] = other.head.data[1];
    head.buffer[1] = other.head.buffer[1];

    head.next = other.head.next;

    other.head.data[0] = other.head.data[1] = nullptr;
    other.head.size[0] = other.head.size[1] = 0;
    other.head.buffer[0] = other.head.buffer[1] = nullptr;
    other.head.next = nullptr;

    other.cleanup_hook = nullptr;
    DEBUG_LOG("move constructor, buffer[0]=0x%x, buffer[1]=0x%x", head.buffer[0], head.buffer[1]);
}


void message::clean_fragment(netio::message::fragment* f)
{
    if(f->next != nullptr)
        clean_fragment(f->next);
    delete f->next;
    f->data[0] = f->data[1] = nullptr;
    f->size[0] = f->size[1] = 0;
    f->buffer[0] = f->buffer[1] = nullptr;
    f->next = nullptr;
}


message::~message()
{
    DEBUG_LOG("~message() (this=0x%x)", this);
    if(cleanup_hook)
        cleanup_hook(*this);

    clean_fragment(&head);
}


message& message::operator= (const message& other)
{
    DEBUG_LOG("message::operator=(const messagse&)");
    if(this != &other)
    {
        if(cleanup_hook)
            cleanup_hook(*this);
        clean_fragment(&head);
        const fragment* p_other = &(other.head);
        fragment* p_this = &head;
        while(p_this != nullptr)
        {
            p_this->data[0] = p_other->data[0];
            p_this->size[0] = p_other->size[0];
            p_this->buffer[0] = p_other->buffer[0];

            p_this->data[1] = p_other->data[1];
            p_this->size[1] = p_other->size[1];
            p_this->buffer[1] = p_other->buffer[1];

            if(p_other->next != nullptr)
                p_this->next = new fragment;
            else
                p_this->next = nullptr;
            p_this = p_this->next;
            p_other = p_other->next;
        }
    }
    return *this;
}


message& message::operator= (message&& other)
{
    DEBUG_LOG("message::operator=(message&&)");
    if(this != &other)
    {
        if(cleanup_hook)
            cleanup_hook(*this);
        clean_fragment(&head);

        cleanup_hook = other.cleanup_hook;
        other.cleanup_hook = nullptr;

        head.data[0] = other.head.data[0];
        head.size[0] = other.head.size[0];
        head.buffer[0] = other.head.buffer[0];

        head.data[1] = other.head.data[1];
        head.size[1] = other.head.size[1];
        head.buffer[1] = other.head.buffer[1];

        head.next = other.head.next;

        other.head.data[0] = other.head.data[1] = nullptr;
        other.head.size[0] = other.head.size[1] = 0;
        other.head.buffer[0] = other.head.buffer[1] = nullptr;
        other.head.next = nullptr;
    }
    return *this;
}


static uint8_t recursive_index(const message::fragment* f, size_t i)
{
    if(f == NULL)
        throw std::runtime_error("index out of bounds");
    if(i < (f->size[0]+f->size[1]))
    {
        if(i<f->size[0])
            return f->data[0][i];
        else
            return f->data[1][i-f->size[0]];
    }
    else
    {
        return recursive_index(f->next, i-(f->size[0]+f->size[1]));
    }
}


uint8_t message::operator[](size_t i) const
{
    return recursive_index(&head, i);
}


void message::add_fragment(const uint8_t* data, size_t size, reusable_buffer* rb)
{
    fragment* p = &head;

    if(p->data[0])
    {
        while(p->next != nullptr) p = p->next;

        if(p->data[1] == nullptr)
        {
            p->data[1] = data;
            p->size[1] = size;
            p->buffer[1] = rb;

            return;
        }
        else
        {
            p->next = new fragment;
            p = p->next;
        }
    }

    p->data[0] = data;
    p->size[0] = size;
    p->buffer[0] = rb;

    p->data[1] = nullptr;
    p->size[1] = 0;
    p->buffer[1] = nullptr;

    p->next = nullptr;
}


const message::fragment* message::fragment_list() const
{
    DEBUG_LOG("Accessing fragment list, head buffer[0]=0x%x, "
              "head buffer[1]=0x%x", head.buffer[0], head.buffer[1]);
    return &head;
}


size_t message::num_fragments() const
{
    size_t n = 0;
    const fragment* p = &head;
    while(p != nullptr)
    {
        n += (p->data[1]? 2: 1);
        p = p->next;
    }
    return n;
}


size_t message::size(void) const
{
    size_t n = 0;
    const fragment* p = &head;
    while(p != nullptr)
    {
        n += (p->size[0] + p->size[1]);
        p = p->next;
    }
    return n;
}


void message::copy_data_to_buffer(uint8_t* buffer) const
{
    DEBUG_LOG("copy_data_to_buffer");
    for(const fragment* p = &head; p != nullptr; p = p->next)
    {
        memcpy(buffer, p->data[0], p->size[0]);
        buffer += p->size[0];

        if(p->size[1])
        {
            memcpy(buffer, p->data[1], p->size[1]);
            buffer += p->size[1];
        }
    }
}


std::vector<uint8_t> message::data_copy(void) const
{
    std::vector<uint8_t> d(this->size());
    copy_data_to_buffer(d.data());
    return d;
}


void message::serialize_to_usr_buffer(void* buffer)
{
    copy_data_to_buffer((uint8_t*)buffer);
    size_t s = this->size();
    if(cleanup_hook)
        cleanup_hook(*this);
    cleanup_hook = NULL;

    clean_fragment(&head);

    head.data[0] = (uint8_t*)buffer;
    head.size[0] = s;
    head.buffer[0] = nullptr;

    head.data[1] = nullptr;
    head.size[1] = 0;
    head.buffer[1] = nullptr;

    head.next = nullptr;
}


void message_cleanup_delete_data(message& m)
{
    const message::fragment* p = m.fragment_list();
    while(p != nullptr)
    {
        delete[] p->data[0];
        if(p->data[1])
            delete[] p->data[1];
        p = p->next;
    }
}

}
