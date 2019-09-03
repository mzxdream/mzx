#include <fcntl.h>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/net/net_socket.h>

namespace mzx
{

NetSocketID NetSocket::CreateTcpSocket(bool is_ipv6, int flags, Error *error)
{
    auto sock = socket(is_ipv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
    if (sock == kNetSocketIDInvalid)
    {
        if (error)
        {
            *error = ToError(errno);
        }
        return kNetSocketIDInvalid;
    }
    auto err = SetFlags(sock, flags);
    if (err != Error::kSuccess)
    {
        if (error)
        {
            *error = ToError(errno);
        }
        DestroySocket(sock);
        return kNetSocketIDInvalid;
    }
    return sock;
}

NetSocketID NetSocket::CreateTcpSocket(const NetAddress &addr, int flags,
                                       Error *error)
{
    auto sock = CreateTcpSocket(addr.IsIPv6(), flags, error);
    if (sock != kNetSocketIDInvalid)
    {
        auto err = Bind(sock, addr);
        if (err != Error::kSuccess)
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

Error NetSocket::Bind(NetSocketID sock, const NetAddress &addr)
{
    if (bind(sock, addr.Address(), addr.Length()) == -1)
    {
        MZX_ERR("bind sock:", sock, " ip:", addr.IP(), " port:", addr.Port(),
                " failed");
        return ToError(errno);
    }
    return Error::kSuccess;
}

Error NetSocket::Listen(NetSocketID sock, int backlog)
{
    if (listen(sock, backlog) == -1)
    {
        MZX_ERR("listen socket:", sock, " backlog:", backlog, " failed");
        return ToError(errno);
    }
    return Error::kSuccess;
}

NetSocketID NetSocket::Accept(NetSocketID sock, NetAddress *addr, int flags,
                              Error *error)
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
            *error = ToError(errno);
        }
        return kNetSocketIDInvalid;
    }
    auto err = SetFlags(sock, flags);
    if (err != Error::kSuccess)
    {
        if (error)
        {
            *error = ToError(errno);
        }
        DestroySocket(accept_sock);
        return kNetSocketIDInvalid;
    }
    return accept_sock;
}

Error NetSocket::Connect(NetSocketID sock, const NetAddress &addr)
{
    if (connect(sock, addr.Address(), addr.Length()) == -1)
    {
        return ToError(errno);
    }
    return Error::kSuccess;
}

int NetSocket::Read(NetSocketID sock, char *data, std::size_t size,
                    Error *error)
{
    MZX_CHECK(data != nullptr && size > 0);
    auto ret = read(sock, data, size);
    if (ret == -1)
    {
        if (error)
        {
            *error = ToError(errno);
        }
    }
    return ret;
}

int NetSocket::Write(NetSocketID sock, const char *data, std::size_t size,
                     Error *error)
{
    MZX_CHECK(data != nullptr && size > 0);
    auto ret = write(sock, data, size);
    if (ret == -1)
    {
        if (error)
        {
            *error = ToError(errno);
        }
    }
    return ret;
}

Error NetSocket::SetNonBlock(NetSocketID sock)
{
    auto flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        MZX_ERR("get sock:", sock, " flags failed");
        return Error::kUnknown;
    }
    flags |= O_NONBLOCK;
    flags = fcntl(sock, F_SETFL, flags);
    if (flags == -1)
    {
        MZX_ERR("set sock:", sock, " flags failed");
        return Error::kUnknown;
    }
    return Error::kSuccess;
}

Error NetSocket::SetCloseOnExec(NetSocketID sock)
{
    auto flags = fcntl(sock, F_GETFD, 0);
    if (flags == -1)
    {
        MZX_ERR("get sock:", sock, " flags failed");
        return Error::kUnknown;
    }
    flags |= FD_CLOEXEC;
    flags = fcntl(sock, F_SETFD, flags);
    if (flags == -1)
    {
        MZX_ERR("set sock:", sock, " flags failed");
        return Error::kUnknown;
    }
    return Error::kSuccess;
}

Error NetSocket::SetReuseAddr(NetSocketID sock)
{
    int enable = 1;
    auto ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (ret < 0)
    {
        MZX_ERR("set sock:", sock, " reuse failed");
        return Error::kUnknown;
    }
    return Error::kSuccess;
}

Error NetSocket::SetFlags(NetSocketID sock, int flags)
{
    if (flags <= 0)
    {
        return Error::kSuccess;
    }
    Error err = Error::kSuccess;
    if ((flags & kNetSocketFlagNonBlock) &&
        (err = NetSocket::SetNonBlock(sock)) != Error::kSuccess)
    {
        MZX_ERR("set sock:", sock, " non block failed");
        return err;
    }
    if ((flags & kNetSocketFlagCloseOnExec) &&
        (err = NetSocket::SetCloseOnExec(sock)) != Error::kSuccess)
    {
        MZX_ERR("set sock:", sock, " close on exec failed");
        return err;
    }
    if ((flags & kNetSocketFlagReuseAddr) &&
        (err = NetSocket::SetReuseAddr(sock)) != Error::kSuccess)
    {
        MZX_ERR("set sock:", sock, " reuse addr failed");
        return err;
    }
    return err;
}

} // namespace mzx
