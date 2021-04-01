#include "serialization.hpp"

#include <cstring>
#include <algorithm>


int
netio::serialize_header_to_buffer(char* dest, size_t dest_len,
                                  netio::msgheader header)
{
    if(dest_len < sizeof(header)+1)
        return 0;

    memcpy(dest, (char*)(&header), sizeof(header));
    dest += sizeof(header);
    dest_len -= sizeof(header);
    return sizeof(header);
}


size_t
netio::serialize_to_buffer(char* dest, size_t dest_len, const char* src, size_t buffer_size,
                           unsigned int* offset)
{
    size_t bytes_to_write = std::min((long long unsigned) buffer_size-*offset,
                                     (long long unsigned) dest_len);
    memcpy(dest, src+*offset, bytes_to_write);
    *offset += bytes_to_write;

    return bytes_to_write;
}
