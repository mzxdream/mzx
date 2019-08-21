#ifndef __MZX_NET_SOCKET_H__
#define __MZX_NET_SOCKET_H__

#include <mzx/net/net_address.h>
#include <mzx/net/net_error.h>

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
                                       NetError *error = nullptr,
                                       int flags = 0);
    static NetSocketID CreateTcpSocket(const NetAddress &addr,
                                       NetError *error = nullptr,
                                       int flags = 0);
    static void DestroySocket(NetSocketID sock);
    static NetError Bind(NetSocketID sock, const NetAddress &addr);
    static NetError Listen(NetSocketID sock, int backlog = 128);
    static NetSocketID Accept(NetSocketID sock, int flags = 0,
                              NetError *error = nullptr,
                              NetAddress *addr = nullptr);
    static NetError Connect(NetSocketID sock, const NetAddress &addr);
    static int Read(NetSocketID sock, char *data, std::size_t size,
                    NetError *error = nullptr);
    static int Write(NetSocketID sock, const char *data, std::size_t size,
                     NetError *error = nullptr);

    static NetError SetNonBlock(NetSocketID sock);
    static NetError SetCloseOnExec(NetSocketID sock);
    static NetError SetReuseAddr(NetSocketID sock);
    static NetError SetFlags(NetSocketID sock, int flags = 0);
};

} // namespace mzx

#endif
