#include <mzx/aio/net/tcp_acceptor.h>
#include <mzx/logger.h>

namespace mzx
{

TcpAcceptor::TcpAcceptor(AIOServer &aio_server)
    : aio_server_(aio_server)
{
}

TcpAcceptor::TcpAcceptor(AIOServer &aio_server, const NetAddress &addr,
                         bool reuse_addr)
    : aio_server_(aio_server)
    , sock_(addr, reuse_addr)
{
    Listen();
}

TcpAcceptor::~TcpAcceptor()
{
    if (aio_handler_)
    {
        delete aio_handler_;
    }
}

AIOServer &TcpAcceptor::GetAIOServer()
{
    return aio_server_;
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
    if (forcePost)
    {
        aio_server_.Post(std::bind(&TcpAcceptor::OnAddAccept, this, sock, cb));
    }
    else
    {
        aio_server_.Exec(std::bind(&TcpAcceptor::OnAddAccept, this, sock, cb));
    }
}

AIOHandler &TcpAcceptor::GetHandler()
{
    if (!aio_handler_)
    {
        aio_handler_ = new AIOHandler(aio_server_, sock_.GetSock());
    }
    return *aio_handler_;
}

void TcpAcceptor::OnCanAccept()
{
    while (!accept_list_.empty())
    {
        auto &info = accept_list_.front();
        if (!sock_.Accept(info.first))
        {
            break;
        }
        (info.second)(Error());
    }
    if (accept_list_.empty())
    {
        GetHandler().EnableRead(false);
    }
}

void TcpAcceptor::OnAddAccept(TcpSocket *sock, AcceptCallback cb)
{
    if (accept_list_.empty())
    {
        if (sock_.Accept(sock))
        {
            cb(Error());
        }
        else
        {
            GetHandler().SetReadCallback(
                std::bind(&TcpAcceptor::OnCanAccept, this));
            GetHandler().EnableRead();
        }
    }
    accept_list_.emplace_back(sock, cb);
}

} // namespace mzx
