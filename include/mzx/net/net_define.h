#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <mzx/logger.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

namespace mzx
{

/*
 * type 0xF | worker 0xFFF | incr 0xFFFF | index 0xFFFFFFFF
 */
using NetConnectionID = uint64_t;
constexpr NetConnectionID kNetConnectionIDInvalid = (NetConnectionID)-1;

enum class NetConnectionType : char
{
    kConnector = 0,
    kPeerConnector = 1,
    kAcceptor = 2,
};

inline NetConnectionType ParseNetConnectionType(NetConnectionID id)
{
    return (NetConnectionType)(id >> 56);
}

inline std::size_t ParseNetConnectionIndex(NetConnectionID id)
{
    return (std::size_t)(id & 0xFFFFFFFF);
}

inline std::size_t ParseNetWorkerIndex(NetConnectionID id)
{
    return (std::size_t)((id >> 48) & 0xFFF);
}

inline NetConnectionID InitNetConnectionID(NetConnectionType type,
                                           std::size_t index)
{
    MZX_CHECK(index <= 0xFFFFFFFF);
    return (((NetConnectionID)type) << 56) + (index & 0xFFFFFFFF);
}

inline NetConnectionID NextNetConnectionID(NetConnectionID id)
{
    auto incr = (((id >> 32) + 1) & 0xFFFFFFF) << 32;
    return (id & 0xF0000000FFFFFFFF) + incr;
}

enum class NetError
{
    kSuccess,
    kUnknown,
};

inline NetError ToNetError(int error)
{
    switch (error)
    {
        case 0:
            return NetError::kSuccess;
        default:
            return NetError::kUnknown;
    }
}

struct NetBuffer
{
    char *data;
    std::size_t size;
    std::size_t start_pos;
    std::size_t end_pos;
};

class NetAddress final
{
public:
    explicit NetAddress()
    {
        memset(&address_, 0, sizeof(address_));
    }
    explicit NetAddress(short port, const char *ip = nullptr,
                        bool is_ipv6 = false)
    {
        memset(&address_, 0, sizeof(address_));
        if (is_ipv6)
        {
            auto *addr6 = (sockaddr_in6 *)&address_;
            addr6->sin6_family = AF_INET6;
            if (!ip || *ip == '\0')
            {
                addr6->sin6_addr = in6addr_any;
            }
            else
            {
                inet_pton(AF_INET6, ip, &addr6->sin6_addr);
            }
            addr6->sin6_port = htons(port);
        }
        else
        {
            auto *addr4 = (sockaddr_in *)&address_;
            addr4->sin_family = AF_INET;
            if (!ip || *ip == '\0')
            {
                addr4->sin_addr.s_addr = htonl(INADDR_ANY);
            }
            else
            {
                inet_pton(AF_INET, ip, &addr4->sin_addr);
            }
            addr4->sin_port = htons(port);
        }
    }

public:
    sockaddr *Address()
    {
        return (sockaddr *)&address_;
    }
    const sockaddr *Address() const
    {
        return (const sockaddr *)&address_;
    }
    socklen_t Length() const
    {
        return sizeof(address_);
    }
    bool IsIPv6() const
    {
        return address_.ss_family == AF_INET6;
    }
    std::string IP() const
    {
        char ip[NI_MAXHOST];
        getnameinfo((sockaddr *)&address_, sizeof(address_), ip, sizeof(ip),
                    nullptr, 0, NI_NUMERICHOST);
        return ip;
    }
    short Port() const
    {
        if (IsIPv6())
        {
            return ntohs(((sockaddr_in6 *)&address_)->sin6_port);
        }
        else
        {
            return ntohs(((sockaddr_in *)&address_)->sin_port);
        }
    }

private:
    sockaddr_storage address_;
};

} // namespace mzx

#endif
