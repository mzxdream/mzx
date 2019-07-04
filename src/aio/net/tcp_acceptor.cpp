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
    aio_handler_.SetFD(sock_.GetSock());
    aio_handler_.SetCloseCallback(
        std::bind(&TcpAcceptor::OnClose, this, std::placeholders::_1));
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
        aio_handler_.SetFD(sock_.GetSock());
        aio_handler_.SetCloseCallback(
            std::bind(&TcpAcceptor::OnClose, this, std::placeholders::_1));
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

void TcpAcceptor::Close()
{
    sock_.Close();
}

void TcpAcceptor::AsyncAccept(TcpConnector *conn, AcceptCallback cb)
{
    MZX_CHECK(conn != nullptr && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpAcceptor::OnAddAccept, this, conn, cb));
}

void TcpAcceptor::OnAddAccept(TcpConnector *conn, AcceptCallback cb)
{
    if (accept_list_.empty())
    {
        while (true)
        {
            auto error = sock_.Accept(&conn->sock_);
            if (!error)
            {
                conn->is_connected_ = true;
                cb(error);
                return;
            }
            if (error.Type() == ErrorType::Interrupt ||
                error.Type() == ErrorType::ConnectAbort)
            {
                continue;
            }
            if (error.Type() == ErrorType::Again)
            {
                aio_handler_.SetReadCallback(
                    std::bind(&TcpAcceptor::OnAccept, this));
                aio_handler_.EnableRead();
                break;
            }
            cb(error);
            OnClose(error);
            return;
        }
    }
    accept_list_.emplace_back(conn, cb);
}

void TcpAcceptor::OnAccept()
{
    while (!accept_list_.empty())
    {
        auto &info = accept_list_.front();
        while (true)
        {
            auto error = sock_.Accept(&info.connector->sock_);
            if (!error)
            {
                info.connector->is_connected_ = true;
                (info.callback)(error);
                break;
            }
            if (error.Type() == ErrorType::Interrupt ||
                error.Type() == ErrorType::ConnectAbort)
            {
                continue;
            }
            if (error.Type() == ErrorType::Again)
            {
                return;
            }
            OnClose(error);
            return;
        }
        accept_list_.pop_front();
    }
    if (accept_list_.empty())
    {
        // aio_handler_.EnableRead(false);
    }
}

void TcpAcceptor::OnClose(const Error &error)
{
    for (auto &info : accept_list_)
    {
        (info.callback)(error);
    }
    accept_list_.clear();
    aio_handler_.EnableAll(false);
    sock_.Close();
}

} // namespace mzx
