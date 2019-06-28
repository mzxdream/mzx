#include <arpa/inet.h>
#include <cstring>

#include <mzx/aio/net/net_define.h>

namespace mzx
{

NetAddress::NetAddress()
{
    memset(&address_, 0, sizeof(address_));
}

NetAddress::NetAddress(uint16_t port, const char *addr, bool is_ipv6)
{
    memset(&address_, 0, sizeof(address_));
    if (is_ipv6)
    {
        auto *addr6 = (sockaddr_in6 *)&address_;
        addr6->sin6_family = AF_INET6;
        if (!addr)
        {
            addr6->sin6_addr = in6addr_any;
        }
        else
        {
            inet_pton(AF_INET6, addr, &addr6->sin6_addr);
        }
        addr6->sin6_port = htons(port);
    }
    else
    {
        auto *addr4 = (sockaddr_in *)&address_;
        addr4->sin_family = AF_INET;
        if (!addr)
        {
            addr4->sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else
        {
            inet_pton(AF_INET, addr, &addr4->sin_addr);
        }
        addr4->sin_port = htons(port);
    }
}

NetAddress::~NetAddress()
{
}

sockaddr *NetAddress::Address()
{
    return (sockaddr *)&address_;
}

const sockaddr *NetAddress::Address() const
{
    return (sockaddr *)&address_;
}

socklen_t NetAddress::Length() const
{
    return sizeof(address_);
}

bool NetAddress::IsIPv6() const
{
    return address_.ss_family == AF_INET6;
}

} // namespace mzx
