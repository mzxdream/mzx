#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/logger.h>

namespace mzx
{

static bool SetNonBlock(int fd)
{
    MZX_CHECK(fd < 0);
    auto flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    auto ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
    {
        MZX_ERR("set non block fd:", fd, " failed");
        return false;
    }
    return true;
}

#if 0
static bool SetCloseOnExec(int fd)
{
    MZX_CHECK(fd < 0);
    auto flags = fcntl(fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    auto ret = fcntl(fd, F_SETFD, flags);
    if (ret == -1)
    {
        MZX_ERR("set close on exec fd:", fd, " failed");
        return false;
    }
    return true;
}
#endif

TcpSocket::TcpSocket(AIOServer &aio_server)
    : aio_server_(aio_server)
{
}

TcpSocket::TcpSocket(AIOServer &aio_server, bool is_ipv6)
    : aio_server_(aio_server)
{
    Open(is_ipv6);
}

TcpSocket::TcpSocket(AIOServer &aio_server, const NetAddress &addr)
    : aio_server_(aio_server)
{
    Open(addr.IsIPv6());
    Bind(addr);
}

TcpSocket::~TcpSocket()
{
    Close();
}

AIOServer &TcpSocket::GetAIOServer()
{
    return aio_server_;
}

bool TcpSocket::IsOpen() const
{
    return sock_ >= 0;
}

bool TcpSocket::Open(bool is_ipv6)
{
    if (IsOpen())
    {
        MZX_ERR("socket:", sock_, " is opened");
        return false;
    }
    if (is_ipv6)
    {
        sock_ = socket(PF_INET6, SOCK_STREAM, 0);
    }
    else
    {
        sock_ = socket(PF_INET, SOCK_STREAM, 0);
    }
    if (sock_ < 0)
    {
        MZX_ERR("create socket failed err:", sock_);
        return false;
    }
    if (!SetNonBlock(sock_))
    {
        MZX_ERR("set socket:", sock_, " non block failed");
        Close();
        return false;
    }
    return true;
}

void TcpSocket::Close()
{
    if (IsOpen())
    {
        close(sock_);
        sock_ = -1;
    }
}

bool TcpSocket::Bind(const NetAddress &addr)
{
    auto ret = bind(sock_, addr.Address(), addr.Length());
    if (ret < 0)
    {
        MZX_ERR("bind socket:", sock_, " failed error:", ret);
        return false;
    }
    return true;
}

bool TcpSocket::SetReuseAddr()
{
    int enable = 1;
    auto ret =
        setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (ret < 0)
    {
        MZX_ERR("set socket:", sock_, " reuse failed err:", ret);
        return false;
    }
    return true;
}

bool TcpSocket::Listen(int backlog)
{
    auto ret = listen(sock_, backlog);
    if (ret < 0)
    {
        MZX_ERR("listen socket:", sock_, " backlog:", backlog,
                " failed err:", ret);
        return false;
    }
    return true;
}

bool TcpSocket::Accept(TcpSocket *sock, NetAddress *addr)
{
    MZX_CHECK(sock != nullptr && !sock->IsOpen());
    int accept_sock = -1;
    if (addr)
    {
        auto len = addr->Length();
        accept_sock = accept(sock_, addr->Address(), &len);
    }
    else
    {
        accept_sock = accept(sock_, nullptr, nullptr);
    }
    if (accept_sock < 0)
    {
        return false;
    }
    if (!SetNonBlock(accept_sock))
    {
        MZX_ERR("set socker:", accept_sock, " non block failed");
        close(accept_sock);
        return false;
    }
    sock->sock_ = accept_sock;
    return true;
}

} // namespace mzx
