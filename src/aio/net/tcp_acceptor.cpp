#include <mzx/aio/net/tcp_acceptor.h>
#include <mzx/logger.h>

namespace mzx
{

TcpAcceptor::TcpAcceptor(AIOServer &aio_server)
    : sock_(aio_server)
{
}

TcpAcceptor::TcpAcceptor(AIOServer &aio_server, const NetAddress &addr,
                         bool reuse_addr)
    : sock_(aio_server)
{
    Open(addr.IsIPv6());
    if (reuse_addr)
    {
        SetReuseAddr();
    }
    Bind(addr);
    Listen();
}

TcpAcceptor::~TcpAcceptor()
{
}

AIOServer &TcpAcceptor::GetAIOServer()
{
    return sock_.GetAIOServer();
}

bool TcpAcceptor::Open(bool is_ipv6)
{
    return sock_.Open(is_ipv6);
}

bool TcpAcceptor::Bind(const NetAddress &addr)
{
    return sock_.Bind(addr);
}

bool TcpAcceptor::SetReuseAddr()
{
    return sock_.SetReuseAddr();
}

bool TcpAcceptor::Listen(int backlog)
{
    return sock_.Listen(backlog);
}

void TcpAcceptor::AsyncAccept(TcpSocket *sock, AcceptCallback cb,
                              bool forcePost)
{
    MZX_CHECK(sock != nullptr && cb != nullptr);
}

} // namespace mzx
