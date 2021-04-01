#pragma once

#include "netio/netio.hpp"


//#define TEST_IT_DESERIALIZATION
#ifdef TEST_IT_DESERIALIZATION
# define DEBUG_LOG( ... ) do { printf("[deserialization@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif

namespace netio
{

/**
 * The deserializer is used to extract encoded messages from pages. Messages
 * will be either stored in the internal message queue, or will passed on to
 * a callback if a callback has been specified.
 *
 * If a callback is registered, messages are *not* enqueued in the internal
 * message queue.
 *
 * The deserializer class *must not* be used directly, but only via one of its
 * subclasses.
 */
class new_deserializer
{
public:

    void feed(netio::reusable_buffer* page)
    {
        size_t size = page->buffer()->pos();
        DEBUG_LOG("feeding page of size %llu", size);
        for(size_t pos=0; pos < size; )
        {
            unsigned bytes_to_consume = std::min(bytes_consumable(), size-pos);
            uint8_t* d = (uint8_t*)page->buffer()->data()+pos;
            DEBUG_LOG("bytes to consume: %llu", bytes_to_consume);
            if(state == CONSUMING)
            {
                add_fragment_to_msg(d, bytes_to_consume, page);
            }
            else if(state == NEW_MESSAGE)
            {
                if(offset == 0 && bytes_to_consume == sizeof(msgheader))
                {
                    header = *((msgheader*)d);
                }
                else
                {
                    memcpy(((char*)&header)+offset, d, bytes_to_consume);
                }
            }

            consume(bytes_to_consume);
            pos += bytes_to_consume;
        }

        page_processed(page);
    }

    // queue mode
    bool try_pop(netio::message* msg)
    {
        return message_queue.try_pop(*msg);
    }

    // callback mode
    typedef void (*callback_t)(netio::message&, void*);
    void register_cb_on_msg_received(callback_t cb, void* data)
    {
        cb_on_msg_received = cb;
        cb_data = data;
    }

protected:
    callback_t cb_on_msg_received = NULL;
    void* cb_data = NULL;
    tbb::concurrent_queue<netio::message> message_queue;
    msgheader header = { 0 };
    size_t offset = 0;
    size_t buflen = 0;

    enum State
    {
        NEW_MESSAGE,
        CONSUMING
    } state = NEW_MESSAGE;


    size_t bytes_consumable()
    {
        switch(state)
        {
        case NEW_MESSAGE:
            return (sizeof header) - offset;
        case CONSUMING:
            return buflen-offset;
        }
        return 0; // not reachable
    }


    void consume(size_t bytes)
    {
        switch(state)
        {
        case NEW_MESSAGE:
            DEBUG_LOG("State: NEW MESSAGE");
            offset += bytes;
            if(offset > sizeof header)
            {
                throw std::runtime_error("Consumed more bytes than available in header");
            }
            else if(offset == sizeof header)
            {
                prepare_new_msg(header.len);
                DEBUG_LOG("Preparing new message of len %llu", header.len);
                state = CONSUMING;
                offset = 0;
                buflen = header.len;
            }
            break;
        case CONSUMING:
            DEBUG_LOG("State: CONSUMING");
            offset += bytes;
            if (offset > buflen)
            {
                throw std::runtime_error("Consumed more than available space in message");
            }
            else if (offset == buflen)
            {
                build_msg();

                state = NEW_MESSAGE;
                offset = 0;
            }
            break;
        }
    }

    // to be implemented by subclasses
    virtual void prepare_new_msg(size_t size) = 0;
    virtual void add_fragment_to_msg(uint8_t* data, size_t size, reusable_buffer* page) = 0;
    virtual void build_msg() = 0;
    virtual void page_processed(reusable_buffer* page) = 0;

    void send_msg(netio::message&& msg)
    {
        netio::message x = std::move(msg);
        if(cb_on_msg_received)
        {
            DEBUG_LOG("Passing message 0x%x to callback", &msg);
            cb_on_msg_received(x, cb_data);
        }
        else
        {
            DEBUG_LOG("Enqueuing message");
            message_queue.push(std::move(x));
        }
    }

};



class callback_deserializer : public new_deserializer
{
public:
    callback_deserializer(msg_rcvd_cb_t callback, void* user_data)  : cb(callback),
        user_data(user_data) {}

protected:
    msg_rcvd_cb_t cb;
    uint8_t* data = nullptr;
    size_t pos = 0;
    size_t size = 0;
    void* user_data;

    void prepare_new_msg(size_t size)
    {
        this->size = size;
        pos = 0;
    }

    void call_callback(uint8_t* data, size_t size)
    {
        cb(data, size, user_data);
    }

    void add_fragment_to_msg(uint8_t* fragment, size_t s, reusable_buffer* page)
    {
        DEBUG_LOG("callback deserializer: add_fragment_to_msg");
        if(s == this->size)
        {
            DEBUG_LOG("full message, calling callback");
            call_callback(fragment, s);
        }
        else
        {
            DEBUG_LOG("partial message");
            if(pos == 0)
            {
                DEBUG_LOG("allocating buffer");
                data = new uint8_t[this->size];
            }
            memcpy(data+pos, fragment, s);
            pos += s;
            if(pos == this->size)
            {
                DEBUG_LOG("partial message -> calling callback");
                call_callback(data, this->size);
                delete[] data;
            }
        }
    }

    void build_msg()
    {
    }

    void page_processed(reusable_buffer* page)
    {
    }
};



class copy_deserializer : public new_deserializer
{
public:
protected:
    uint8_t* data = nullptr;
    size_t pos = 0;

    void prepare_new_msg(size_t size)
    {
        DEBUG_LOG("Allocating buffer of size %ull", size);
        data = new uint8_t[size];
        pos = 0;
        if(!data)
            throw std::runtime_error("Could not allocate memory");
    }

    void add_fragment_to_msg(uint8_t* fragment, size_t size, reusable_buffer* page)
    {
        DEBUG_LOG("Copying fragment of size %llu, data=%u, pos=%u", size, data, pos);
        memcpy(data+pos, fragment, size);
        pos += size;
    }

    void build_msg()
    {
        DEBUG_LOG("Building message of size %llu", pos);
        netio::message msg = message(data, pos, nullptr, message_cleanup_delete_data);
        send_msg(std::move(msg));
    }

    void page_processed(reusable_buffer* page)
    {
    }
};


inline void reference_deserializer_cleanup_hook(netio::message& m)
{
    DEBUG_LOG("Calling cleanup hook on msg 0x%x", &m);

    const netio::message::fragment* p = m.fragment_list();

    while(p != nullptr)
    {
        DEBUG_LOG("Releasing page 0x%x", p->buffer[0]);
        p->buffer[0]->dec_refcount();
        if(p->buffer[1])
        {
            DEBUG_LOG("Releasing page 0x%x", p->buffer[1]);
            p->buffer[1]->dec_refcount();
        }
        p = p->next;
    }
}

class reference_deserializer : public new_deserializer
{
protected:
    netio::message msg;

    void prepare_new_msg(size_t size)
    {
        msg = netio::message(reference_deserializer_cleanup_hook);
    }

    void add_fragment_to_msg(uint8_t* fragment, size_t size, reusable_buffer* page)
    {
        msg.add_fragment(fragment, size, page);
        page->inc_refcount();
        DEBUG_LOG("Adding fragment from page 0x%x", (uint64_t)page);
    }

    void build_msg()
    {
        send_msg(std::move(msg));
    }

    void page_processed(reusable_buffer* page)
    {
    }
};


}
