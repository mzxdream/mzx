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
    aio_handler_.SetFD(sock_.GetSock());
    aio_handler_.SetCloseCallback(
        std::bind(&TcpConnector::OnClose, this, std::placeholders::_1));
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
    if (sock_.Open(is_ipv6))
    {
        aio_handler_.SetFD(sock_.GetSock());
        aio_handler_.SetCloseCallback(
            std::bind(&TcpConnector::OnClose, this, std::placeholders::_1));
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

void TcpConnector::Close()
{
    aio_server_.Exec(std::bind(&TcpConnector::OnCloseActive, this));
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
    MZX_CHECK(data != nullptr && size > 0 && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddWrite, this, data, size, cb),
                     forcePost);
}

void TcpConnector::OnAddConnect(const NetAddress &addr, ConnectCallback cb)
{
    if (connect_list_.empty())
    {
        if (is_connected_)
        {
            cb(Error());
            return;
        }
        while (true)
        {
            auto error = sock_.Connect(addr);
            if (!error)
            {
                is_connected_ = true;
                cb(error);
                return;
            }
            if (error.Type() == ErrorType::Interrupt)
            {
                continue;
            }
            if (error.Type() == ErrorType::InProgress)
            {
                aio_handler_.SetWriteCallback(
                    std::bind(&TcpConnector::OnConnect, this));
                aio_handler_.EnableWrite();
                break;
            }
            cb(error);
            OnClose(error);
            return;
        }
    }
    connect_list_.emplace_back(addr, cb);
}

void TcpConnector::OnAddRead(char *data, std::size_t size, ReadCallback cb)
{
    std::size_t read_size = 0;
    if (read_list_.empty() && is_connected_)
    {
        while (true)
        {
            auto error = sock_.Read(data, size, &read_size);
            if (!error)
            {
                if (read_size < size)
                {
                    aio_handler_.SetReadCallback(
                        std::bind(&TcpConnector::OnRead, this));
                    aio_handler_.EnableRead();
                    break;
                }
                cb(error);
                return;
            }
            if (error.Type() == ErrorType::Interrupt)
            {
                continue;
            }
            if (error.Type() == ErrorType::Again)
            {
                aio_handler_.SetReadCallback(
                    std::bind(&TcpConnector::OnRead, this));
                aio_handler_.EnableRead();
                break;
            }
            cb(error);
            OnClose(error);
            return;
        }
    }
    read_list_.emplace_back(data, size, read_size, cb);
}

void TcpConnector::OnAddWrite(const char *data, std::size_t size,
                              WriteCallback cb)
{
    std::size_t write_size = 0;
    if (write_list_.empty() && is_connected_)
    {
        while (true)
        {
            auto error = sock_.Write(data, size, &write_size);
            if (!error)
            {
                if (write_size < size)
                {
                    aio_handler_.SetWriteCallback(
                        std::bind(&TcpConnector::OnWrite, this));
                    aio_handler_.EnableWrite();
                    break;
                }
                cb(error);
                return;
            }
            if (error.Type() == ErrorType::Interrupt)
            {
                continue;
            }
            if (error.Type() == ErrorType::Again)
            {
                aio_handler_.SetWriteCallback(
                    std::bind(&TcpConnector::OnWrite, this));
                aio_handler_.EnableWrite();
                break;
            }
            cb(error);
            OnClose(error);
            return;
        }
    }
    write_list_.emplace_back(data, size, write_size, cb);
}

void TcpConnector::OnConnect()
{
    is_connected_ = true;
    aio_handler_.EnableWrite(false);
    for (auto &info : connect_list_)
    {
        (info.callback)(Error());
    }
    connect_list_.clear();
    OnRead();
    OnWrite();
}

void TcpConnector::OnRead()
{
    while (!read_list_.empty() && is_connected_)
    {
        std::size_t read_size = 0;
        auto &info = read_list_.front();
        while (true)
        {
            auto error = sock_.Read(info.data + info.read_size,
                                    info.size - info.read_size, &read_size);
            if (!error)
            {
                info.read_size += read_size;
                if (info.read_size < info.size)
                {
                    return;
                }
                (info.callback)(error);
                break;
            }
            if (error.Type() == ErrorType::Interrupt)
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
        read_list_.pop_front();
    }
    if (read_list_.empty())
    {
        // aio_handler_.EnableRead(false);
    }
}

void TcpConnector::OnWrite()
{
    while (!write_list_.empty() && is_connected_)
    {
        std::size_t write_size = 0;
        auto &info = write_list_.front();
        while (true)
        {
            auto error = sock_.Write(info.data + info.write_size,
                                     info.size - info.write_size, &write_size);
            if (!error)
            {
                info.write_size += write_size;
                if (info.write_size < info.size)
                {
                    return;
                }
                (info.callback)(error);
                break;
            }
            if (error.Type() == ErrorType::Interrupt)
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
        write_list_.pop_front();
    }
    if (write_list_.empty())
    {
        aio_handler_.EnableWrite(false);
    }
}

void TcpConnector::OnClose(const Error &error)
{
    for (auto &info : connect_list_)
    {
        (info.callback)(error);
    }
    connect_list_.clear();
    for (auto &info : read_list_)
    {
        (info.callback)(error);
    }
    read_list_.clear();
    for (auto &info : write_list_)
    {
        (info.callback)(error);
    }
    write_list_.clear();
    aio_handler_.EnableAll(false);
}

} // namespace mzx
