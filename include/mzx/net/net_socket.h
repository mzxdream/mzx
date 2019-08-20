#ifndef __MZX_NET_SOCKET_H__
#define __MZX_NET_SOCKET_H__

#include <mzx/net/net_address.h>
#include <mzx/net/net_error.h>

namespace mzx
{

using NetSocketID = int;
constexpr NetSocketID kNetSocketIDInvalid = (NetSocketID)-1;

class NetSocket
{
public:
    static NetSocketID CreateTcpSocket(bool is_ipv6 = false);
    static void DestroySocket(NetSocketID sock);
    static NetError SetNonBlock(NetSocketID sock);
    static NetError SetCloseOnExec(NetSocketID sock);
    static NetError Bind(NetSocketID sock, const NetAddress &addr);
    static NetSocketID CreateTcpAcceptor(const NetAddress &addr,
                                         NetError *error = nullptr);
    static NetError SetReuseAddr(NetSocketID sock);
    static NetError Listen(NetSocketID sock, int backlog = 128);
    static NetSocketID Accept(NetSocketID sock, NetAddress *addr = nullptr,
                              NetError *error = nullptr);
    static NetError Connect(NetSocketID sock, const NetAddress &addr);
    static int Read(NetSocketID sock, char *data, std::size_t size,
                    NetError *error = nullptr);
    static int Write(NetSocketID sock, const char *data, std::size_t size,
                     NetError *error = nullptr);
};

} // namespace mzx

#endif
