#include <fcntl.h>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/net/net_socket.h>

namespace mzx
{

NetSocketID NetSocket::CreateTcpSocket(bool is_ipv6, NetError *error, int flags)
{
    auto sock = socket(is_ipv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
    if (sock == kNetSocketIDInvalid)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
        return kNetSocketIDInvalid;
    }
    auto err = SetFlags(sock, flags);
    if (err != NetError::kSuccess)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
        DestroySocket(sock);
        return kNetSocketIDInvalid;
    }
    return sock;
}

NetSocketID NetSocket::CreateTcpSocket(const NetAddress &addr, NetError *error,
                                       int flags)
{
    auto sock = CreateTcpSocket(addr.IsIPv6(), error, flags);
    if (sock != kNetSocketIDInvalid)
    {
        auto err = Bind(sock, addr);
        if (err != NetError::kSuccess)
        {
            if (error)
            {
                *error = err;
            }
            DestroySocket(sock);
            return kNetSocketIDInvalid;
        }
    }
    return sock;
}

void NetSocket::DestroySocket(NetSocketID sock)
{
    close(sock);
}

NetError NetSocket::Bind(NetSocketID sock, const NetAddress &addr)
{
    if (bind(sock, addr.Address(), addr.Length()) == -1)
    {
        MZX_ERR("bind sock:", sock, " ip:", addr.IP(), " port:", addr.Port(),
                " failed");
        return ToNetError(errno);
    }
    return NetError::kSuccess;
}

NetError NetSocket::Listen(NetSocketID sock, int backlog)
{
    if (listen(sock, backlog) == -1)
    {
        MZX_ERR("listen socket:", sock, " backlog:", backlog, " failed");
        return ToNetError(errno);
    }
    return NetError::kSuccess;
}

NetSocketID NetSocket::Accept(NetSocketID sock, int flags, NetError *error,
                              NetAddress *addr)
{
    NetSocketID accept_sock = kNetSocketIDInvalid;
    if (addr)
    {
        auto len = addr->Length();
        accept_sock = accept(sock, addr->Address(), &len);
    }
    else
    {
        accept_sock = accept(sock, nullptr, nullptr);
    }
    if (accept_sock == kNetSocketIDInvalid)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
        return kNetSocketIDInvalid;
    }
    auto err = SetFlags(sock, flags);
    if (err != NetError::kSuccess)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
        DestroySocket(accept_sock);
        return kNetSocketIDInvalid;
    }
    return accept_sock;
}

NetError NetSocket::Connect(NetSocketID sock, const NetAddress &addr)
{
    if (connect(sock, addr.Address(), addr.Length()) == -1)
    {
        return ToNetError(errno);
    }
    return NetError::kSuccess;
}

int NetSocket::Read(NetSocketID sock, char *data, std::size_t size,
                    NetError *error)
{
    MZX_CHECK(data != nullptr && size > 0);
    auto ret = read(sock, data, size);
    if (ret == -1)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
    }
    return ret;
}

int NetSocket::Write(NetSocketID sock, const char *data, std::size_t size,
                     NetError *error)
{
    MZX_CHECK(data != nullptr && size > 0);
    auto ret = write(sock, data, size);
    if (ret == -1)
    {
        if (error)
        {
            *error = ToNetError(errno);
        }
    }
    return ret;
}

NetError NetSocket::SetNonBlock(NetSocketID sock)
{
    auto flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        MZX_ERR("get sock:", sock, " flags failed");
        return NetError::kUnknown;
    }
    flags |= O_NONBLOCK;
    flags = fcntl(sock, F_SETFL, flags);
    if (flags == -1)
    {
        MZX_ERR("set sock:", sock, " flags failed");
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

NetError NetSocket::SetCloseOnExec(NetSocketID sock)
{
    auto flags = fcntl(sock, F_GETFD, 0);
    if (flags == -1)
    {
        MZX_ERR("get sock:", sock, " flags failed");
        return NetError::kUnknown;
    }
    flags |= FD_CLOEXEC;
    flags = fcntl(sock, F_SETFD, flags);
    if (flags == -1)
    {
        MZX_ERR("set sock:", sock, " flags failed");
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

NetError NetSocket::SetReuseAddr(NetSocketID sock)
{
    int enable = 1;
    auto ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (ret < 0)
    {
        MZX_ERR("set sock:", sock, " reuse failed");
        return NetError::kUnknown;
    }
    return NetError::kSuccess;
}

NetError NetSocket::SetFlags(NetSocketID sock, int flags)
{
    if (flags <= 0)
    {
        return NetError::kSuccess;
    }
    NetError err = NetError::kSuccess;
    if ((flags & kNetSocketFlagNonBlock) &&
        (err = NetSocket::SetNonBlock(sock)) != NetError::kSuccess)
    {
        MZX_ERR("set sock:", sock, " non block failed");
        return err;
    }
    if ((flags & kNetSocketFlagCloseOnExec) &&
        (err = NetSocket::SetCloseOnExec(sock)) != NetError::kSuccess)
    {
        MZX_ERR("set sock:", sock, " close on exec failed");
        return err;
    }
    if ((flags & kNetSocketFlagReuseAddr) &&
        (err = NetSocket::SetReuseAddr(sock)) != NetError::kSuccess)
    {
        MZX_ERR("set sock:", sock, " reuse addr failed");
        return err;
    }
    return err;
}

} // namespace mzx
