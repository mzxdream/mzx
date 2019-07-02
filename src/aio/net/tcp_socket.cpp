#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/logger.h>

namespace mzx
{

TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(bool is_ipv6)
{
    Open(is_ipv6);
}

TcpSocket::TcpSocket(const NetAddress &addr, bool reuse_addr)
{
    Open(addr.IsIPv6());
    if (reuse_addr)
    {
        SetReuseAddr();
    }
    Bind(addr);
}

TcpSocket::~TcpSocket()
{
    Close();
}

int TcpSocket::GetSock() const
{
    return sock_;
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
    if (!SetCloseOnExec(sock_))
    {
        MZX_ERR("set socket:", sock_, " close on exec failed");
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

Error TcpSocket::Accept(TcpSocket *sock, NetAddress *addr)
{
    MZX_CHECK(sock != nullptr && !sock->IsOpen());
    int peer_sock = -1;
    if (addr)
    {
        auto len = addr->Length();
        peer_sock = accept(sock_, addr->Address(), &len);
    }
    else
    {
        peer_sock = accept(sock_, nullptr, nullptr);
    }
    if (peer_sock < 0)
    {
        return Error(errno);
    }
    if (!SetNonBlock(peer_sock))
    {
        MZX_ERR("set socker:", peer_sock, " non block failed");
        close(peer_sock);
        return Error(Error::Unknown);
    }
    if (!SetCloseOnExec(peer_sock))
    {
        MZX_ERR("set socket:", peer_sock, " close on exec failed");
        close(peer_sock);
        return Error(Error::Unknown);
    }
    sock->sock_ = peer_sock;
    return Error();
}

Error TcpSocket::Connect(const NetAddress &addr)
{
    auto ret = connect(sock_, addr.Address(), addr.Length());
    if (ret < 0)
    {
        return Error(errno);
    }
    return Error();
}

Error TcpSocket::Read(char *data, std::size_t size, std::size_t *read_size)
{
    auto ret = read(sock_, data, size);
    if (ret < 0)
    {
        return Error(errno);
    }
    if (read_size)
    {
        *read_size = (std::size_t)ret;
    }
    return Error();
}

Error TcpSocket::Write(const char *data, std::size_t size,
                       std::size_t *write_size)
{
    auto ret = write(sock_, data, size);
    if (ret < 0)
    {
        return Error(errno);
    }
    if (write_size)
    {
        *write_size = (std::size_t)ret;
    }
    return Error();
}

} // namespace mzx
