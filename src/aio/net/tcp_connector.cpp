#include <mzx/aio/net/tcp_connector.h>

namespace mzx
{

TcpConnector::TcpConnector(AIOServer &aio_server)
    : aio_server_(aio_server)
    , aio_handler_(aio_server)
{
}

TcpConnector::TcpConnector(AIOServer &aio_server, const NetAddress &addr,
                           bool reuse_addr)
    : aio_server_(aio_server)
    , sock_(addr, reuse_addr)
    , aio_handler_(aio_server)
{
    aio_handler_.SetFD(sock_.sock());
}

TcpConnector::~TcpConnector()
{
}

AIOServer &TcpConnector::GetAIOServer()
{
    return aio_server_;
}

bool TcpConnector::Open(bool is_ipv6)
{
    if (sock_.open(is_ipv6))
    {
        aio_handler_.SetFD(sock_.sock());
        return true;
    }
    return false;
}

bool TcpConnector::Bind(const NetAddress &addr)
{
    return sock_.Bind(addr);
}

bool TcpConnector::SetReuseAddr()
{
    return sock_.SetReuseAddr();
}

void TcpConnector::AsyncConnect(const NetAddress &addr, ConnectCallback cb,
                                bool forcePost)
{
    MZX_CHECK(cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddConnect, this, addr, cb),
                     forcePost);
}

void TcpConnector::AsyncRead(char *data, std::size_t size, ReadCallback cb,
                             bool forcePost)
{
    MZX_CHECK(data != nullptr && size > 0 && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddRead, this, data, size, cb),
                     forcePost);
}

void TcpConnector::AsyncWrite(const char *data, std::size_t size,
                              WriteCallback cb, bool forcePost)
{
    MZX_CHECK(data != nullptr, &&size > 0 && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddWrite, this, data, size, cb),
                     forcePost);
}

void TcpConnector::OnAddConnect(const NetAddress &addr, ConnectCallback cb)
{
    if (connect_list_.empty())
    {
        if (is_connected)
        {
            cb(Error());
            return;
        }
        auto error = sock_.Connect(addr);
        if (!error)
        {
            cb(Error());
            return;
        }
        if (error.Type() != Error::InProgress)
        {
            cb(error);
            return;
        }
        aio_handler_.SetWriteCallback(
            std::bind(&TcpConnector::OnConnect, this));
        aio_handler_.EnableWrite();
    }
    connect_list_.emplace_back(addr, cb);
}

void TcpConnector::OnAddRead(char *data, std::size_t size, ReadCallback cb)
{
    std::size_t read_size = 0;
    if (read_list_.empty())
    {
        if (is_connected_)
        {
            auto error = sock_.Read(data, size, &read_size);
            if (!error)
            {
                cb(error);
                return;
            }
            if (error.Type() != Error::Again)
            {
                cb(error);
                return;
            }
            aio_handler_.SetReadCallback(
                std::bind(&TcpConnector::OnRead, this));
            aio_handler_.EnableRead();
        }
    }
    read_list_.emplace_back(data, size, read_size, cb);
}

void TcpConnector::OnAddWrite(const char *data, std::size_t size,
                              WriteCallback cb)
{
    std::size_t write_size = 0;
    if (write_list_.empty())
    {
        if (is_connected_)
        {
            auto error = sock_.Write(data, size, &write_size);
            if (!error)
            {
                cb(error);
                return;
            }
            if (error.Type() != Error::Again)
            {
                cb(error);
                return;
            }
            aio_handler_.SetWriteCallback(
                std::bind(&TcpConnector::OnWrite, this));
            aio_handler_.EnableWrite();
        }
    }
    write_list_.emplace_back(data, size, write_size, cb);
}

void TcpConnector::OnConnect()
{

}

void TcpConnector::OnRead()
{
}

void TcpConnector::OnWrite()
{
}

} // namespace mzx
