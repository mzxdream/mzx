#ifndef __MZX_NET_SOCKET_H__
#define __MZX_NET_SOCKET_H__

#include <fcntl.h>
#include <unistd.h>

#include <mzx/net/net_define.h>

namespace mzx
{

inline int CreateTcpSocket(bool is_ipv6 = false, NetError *error = nullptr)
{
    int sock = socket(is_ipv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
    if (error)
    {
        *error = sock == -1 ? NetError::kUnknown : NetError::kSuccess;
    }
    return sock;
}

inline NetError SetNonBlock(int sock)
{
    MZX_CHECK(sock != -1);
    auto flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        return NetError::kUnknown;
    }
    flags |= O_NONBLOCK;
    flags = fcntl(sock, F_SETFL, flags);
    if (flags == -1)
    {
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

inline NetError SetCloseOnExec(int sock)
{
    MZX_CHECK(sock != -1);
    auto flags = fcntl(sock, F_GETFD, 0);
    if (flags == -1)
    {
        return NetError::kUnknown;
    }
    flags |= FD_CLOEXEC;
    flags = fcntl(sock, F_SETFD, flags);
    if (flags == -1)
    {
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

inline NetError Bind(int sock, const NetAddress &addr)
{
    MZX_CHECK(sock != -1);
    if (bind(sock, addr.Address(), addr.Length()) == -1)
    {
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

inline int CreateAcceptor(const NetAddress &addr, NetError *error = nullptr)
{
    int sock = CreateTcpSocket(addr.IsIPv6(), error);
    if (sock == -1)
    {
        return sock;
    }
    NetError err = NetError::kSuccess;
    do
    {
        err = SetNonBlock(sock);
        if (err != NetError::kSuccess)
        {
            break;
        }
        err = SetCloseOnExec(sock);
        if (err != NetError::kSuccess)
        {
            break;
        }
    } while (0);
    if (err != NetError::kSuccess)
    {
        close(sock);
        sock = -1;
    }
    if (error)
    {
        *error = err;
    }
    return sock;
}

} // namespace mzx

#endif
