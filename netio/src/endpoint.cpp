#include "netio/netio.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>



//#define TEST_IT
#ifdef TEST_IT
# define DEBUG_LOG( ... ) do { printf("[endpoint@%s:%3d] ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#else
# define DEBUG_LOG( ... )
#endif


namespace netio
{
endpoint::endpoint(unsigned short port)
{
    //load_sockaddr("0.0.0.0", port);
}

endpoint::endpoint(std::string hostname, unsigned short port)
{
    load_sockaddr(hostname.c_str(), port);
}

endpoint::endpoint(const char* hostname, unsigned short port)
{
    load_sockaddr(hostname, port);
}

endpoint::endpoint(const endpoint& other)
{
    addr = other.addr;
}

bool
endpoint::operator==(const endpoint& other) const
{
    if (addr.ss_family == AF_INET)   // IPv4
    {
        struct sockaddr_in *a_this = (struct sockaddr_in *)(&addr);
        struct sockaddr_in *a_that = (struct sockaddr_in *)(other.sockaddr());
        return (a_this->sin_addr.s_addr == a_that->sin_addr.s_addr)
               && (a_this->sin_port == a_that->sin_port);
    }
    else     // IPv6
    {
        struct sockaddr_in6 *a_this = (struct sockaddr_in6 *)(&addr);
        struct sockaddr_in6 *a_that = (struct sockaddr_in6 *)(other.sockaddr());
        return (memcmp(a_this->sin6_addr.s6_addr, a_that->sin6_addr.s6_addr, sizeof(struct in6_addr)) == 0)
               && (a_this->sin6_port == a_that->sin6_port);
    }
}

bool
endpoint::operator<(const endpoint& other) const
{
    if(addr.ss_family == AF_INET) // IPv4
    {
        if(other.addr.ss_family != AF_INET) return true;  // IPv4 < IPv6
        return memcmp(&addr, &other.addr, sizeof(struct sockaddr_in)) < 0;
    }
    else     // IPv6
    {
        if(other.addr.ss_family == AF_INET) return false; // IPv6 > IPv4
        return memcmp(&addr, &other.addr, sizeof(struct sockaddr_in6)) < 0;
    }
}

const struct sockaddr*
endpoint::sockaddr() const
{
    return (const struct sockaddr*)&addr;
}


struct sockaddr*
endpoint::sockaddr()
{
    return (struct sockaddr*)&addr;
}


std::string
endpoint::address() const
{
    char ipstr[INET6_ADDRSTRLEN];
    void *p;

    if (addr.ss_family == AF_INET)   // IPv4
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)(&addr);
        p = &(ipv4->sin_addr);
    }
    else     // IPv6
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)(&addr);
        p = &(ipv6->sin6_addr);
    }
    inet_ntop(addr.ss_family, p, ipstr, sizeof ipstr);
    DEBUG_LOG("Address string: '%s'", ipstr);
    return std::string(ipstr);
}

unsigned short
endpoint::port() const
{
    if (addr.ss_family == AF_INET)   // IPv4
    {
        DEBUG_LOG("IPv4");
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)(&addr);
        return ntohs(ipv4->sin_port);
    }
    else     // IPv6
    {
        DEBUG_LOG("IPv6");
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)(&addr);
        return ntohs(ipv6->sin6_port);
    }
}

void
endpoint::load_sockaddr(const char* hostname, unsigned short port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *results;

    char portstr[32];
    snprintf(portstr, 32, "%d", port);
    DEBUG_LOG("Port: %d, as str: %s", port, portstr);

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // get ready to connect
    status = getaddrinfo(hostname, portstr, &hints, &results);
    if(status)
    {
        throw std::runtime_error(gai_strerror(status));
    }

    // We do not iterate through the results, we simply take the first one
    memcpy(&addr, results->ai_addr, results->ai_addrlen);
    freeaddrinfo(results);
}

}
