#ifndef __MZX_NET_SOCKET_H__
#define __MZX_NET_SOCKET_H__

#include <mzx/error.h>
#include <mzx/net/net_address.h>

namespace mzx
{

using NetSocketID = int;
constexpr NetSocketID kNetSocketIDInvalid = (NetSocketID)-1;

constexpr int kNetSocketFlagNonBlock = 0x1;
constexpr int kNetSocketFlagCloseOnExec = 0x2;
constexpr int kNetSocketFlagReuseAddr = 0x4;

class NetSocket
{
public:
    static NetSocketID CreateTcpSocket(bool is_ipv6 = false,
                                       Error *error = nullptr, int flags = 0);
    static NetSocketID CreateTcpSocket(const NetAddress &addr,
                                       Error *error = nullptr, int flags = 0);
    static void DestroySocket(NetSocketID sock);
    static Error Bind(NetSocketID sock, const NetAddress &addr);
    static Error Listen(NetSocketID sock, int backlog = 128);
    static NetSocketID Accept(NetSocketID sock, int flags = 0,
                              Error *error = nullptr,
                              NetAddress *addr = nullptr);
    static Error Connect(NetSocketID sock, const NetAddress &addr);
    static int Read(NetSocketID sock, char *data, std::size_t size,
                    Error *error = nullptr);
    static int Write(NetSocketID sock, const char *data, std::size_t size,
                     Error *error = nullptr);

    static Error SetNonBlock(NetSocketID sock);
    static Error SetCloseOnExec(NetSocketID sock);
    static Error SetReuseAddr(NetSocketID sock);
    static Error SetFlags(NetSocketID sock, int flags = 0);
};

} // namespace mzx

#endif
