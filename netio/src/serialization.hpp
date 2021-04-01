#ifndef FELIX_NETIO_SERIALIZATION_HPP
#define FELIX_NETIO_SERIALIZATION_HPP

#include "netio/netio.hpp"

#include <unistd.h>
#include <functional>

namespace netio
{

struct msgheader
{
    uint64_t len;
};



/**
 * Serializes the given header to the given buffer.
 * Returns the number of bytes written. If there is not sufficient space in the
 * buffer, 0 bytes are written.
 */
int serialize_header_to_buffer(char* dest, size_t dest_len, msgheader header);

/**
 * Serializes an input buffer (given in src) to an output buffer. This
 * requires that a header was written to dest previously. The
 * size of the destination buffer is given in dest_len.
 *
 * The function returns the number of bytes of the source buffer that
 * have been serialized. If the serialized version of the source
 * buffer is larger than the size of the destination buffer this
 * number can be smaller than len and subsequent calls are necessary.
 *
 * The function may have to be called sevaral times with different
 * offsets when the destination buffer cannot fit the entire message.
 */
size_t serialize_to_buffer(char* dest, size_t dest_len, const char* src, size_t buffer_size,
                           unsigned int* offset);




class deserializer
{
    msgheader header = { 0 };

    char* buf = NULL;
    size_t offset = 0;
    size_t buflen = 0;

    std::function<void(void*, size_t)> new_message_cb;

    enum State
    {
        NEW_MESSAGE,
        CONSUMING
    } state;

    void create_buffer(size_t size)
    {
        if(size == 0)
            throw std::runtime_error("zero-length message received (currently not supported)");
        buf = new char[size];
        buflen = size;
        offset = 0;
    }

    void prepare_new_message()
    {
        buf = NULL;
        buflen = 0;
        offset = 0;
        header.len = 0;
    }

public:
    deserializer(std::function<void(void*, size_t)>  callback)
        : new_message_cb(callback), state(NEW_MESSAGE) {}

    ~deserializer()
    {
        if(buf)
            delete[] buf;
    }

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
            offset += bytes;
            if(offset > sizeof header)
            {
                throw std::runtime_error("Consumed more bytes than available in header");
            }
            else if(offset == sizeof header)
            {
                create_buffer(header.len);
                state = CONSUMING;
            }
            break;
        case CONSUMING:
            offset += bytes;
            if (offset > buflen)
            {
                throw std::runtime_error("Consumed more than available space in message");
            }
            else if (offset == buflen)
            {
                new_message_cb(buf, buflen);
                prepare_new_message();
                state = NEW_MESSAGE;
            }
            break;
        }
    }

    char* buffer()
    {
        switch(state)
        {
        case NEW_MESSAGE:
            return ((char*)(&header)) + offset;
        case CONSUMING:
            return buf+offset;
        }
        return NULL; // not reachable
    }
};



}

#endif
