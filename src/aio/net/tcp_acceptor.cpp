#include <mzx/aio/net/tcp_acceptor.h>
#include <mzx/logger.h>

namespace mzx
{

TcpAcceptor::TcpAcceptor(AIOServer &aio_server)
    : aio_server_(aio_server)
    , aio_handler_(aio_server)
{
}

TcpAcceptor::TcpAcceptor(AIOServer &aio_server, const NetAddress &addr,
                         bool reuse_addr)
    : aio_server_(aio_server)
    , sock_(addr, reuse_addr)
    , aio_handler_(aio_server)
{
    aio_handler_.SetFD(sock_.sock());
    Listen();
}

TcpAcceptor::~TcpAcceptor()
{
}

AIOServer &TcpAcceptor::GetAIOServer()
{
    return aio_server_;
}

bool TcpAcceptor::Open(bool is_ipv6)
{
    if (sock_.Open(is_ipv6))
    {
        aio_handler_.SetFD(sock_.sock());
        return true;
    }
    return false;
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

void TcpAcceptor::AsyncAccept(TcpConnector *conn, AcceptCallback cb,
                              bool forcePost)
{
    MZX_CHECK(conn != nullptr && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpAcceptor::OnAddAccept, this, conn, cb),
                     forcePost);
}

void TcpAcceptor::OnAddAccept(TcpConnector *conn, AcceptCallback cb)
{
    if (accept_list_.empty())
    {
        auto error = sock_.Accept(&conn->sock_);
        if (!error)
        {
            conn->is_connected_ = true;
            cb(error);
            return;
        }
        if (error.Type() != Error::Again &&
            error.Type() != Error::ConnetAbort && error.Type() != Error::Proto)
        {
            cb(error);
            return;
        }
        aio_handler_.SetReadCallback(std::bind(&TcpAcceptor::OnAccept, this));
        aio_handler_.EnableRead();
    }
    accept_list_.emplace_back(sock, cb);
}

void TcpAcceptor::OnAccept()
{
    while (!accept_list_.empty())
    {
        auto &info = accept_list_.front();
        auto error = sock_.Accept(&info.conn->sock_);
        if (error)
        {
            if (error != Error::Again)
            {
                // TODO
            }
            break;
        }
        info.conn->is_connected = true;
        (info.cb)(error);
        accept_list_.pop_front();
    }
    if (accept_list_.empty())
    {
        aio_handler_.EnableRead(false);
    }
}

} // namespace mzx
