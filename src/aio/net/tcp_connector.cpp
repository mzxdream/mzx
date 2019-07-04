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
    sock_.Close();
}

void TcpConnector::AsyncConnect(const NetAddress &addr, ConnectCallback cb)
{
    MZX_CHECK(cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddConnect, this, addr, cb));
}

void TcpConnector::AsyncRead(char *data, std::size_t size, ReadCallback cb)
{
    MZX_CHECK(data != nullptr && size > 0 && cb != nullptr);
    aio_server_.Exec(std::bind(&TcpConnector::OnAddRead, this, data, size, cb));
}

void TcpConnector::AsyncWrite(const char *data, std::size_t size,
                              WriteCallback cb)
{
    MZX_CHECK(data != nullptr && size > 0 && cb != nullptr);
    aio_server_.Exec(
        std::bind(&TcpConnector::OnAddWrite, this, data, size, cb));
}

void TcpConnector::OnAddConnect(const NetAddress &addr, ConnectCallback cb)
{
    if (is_connected_ || connect_cb_)
    {
        cb(Error(ErrorType::RepeatConnect));
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
        if (error.Type() == ErrorType::InProgress ||
            error.Type() == ErrorType::Again)
        {
            remote_addr_ = addr;
            connect_cb_ = cb;
            aio_handler_.SetWriteCallback(
                std::bind(&TcpConnector::OnConnect, this));
            aio_handler_.EnableWrite();
            return;
        }
        cb(error);
        OnClose(error);
        return;
    }
}

void TcpConnector::OnAddRead(char *data, std::size_t size, ReadCallback cb)
{
    if (!is_connected_)
    {
        cb(ErrorType::NotConnected);
        return;
    }
    std::size_t read_size = 0;
    if (read_list_.empty())
    {
        while (true)
        {
            auto error = sock_.Read(data, size, &read_size);
            if (!error)
            {
                if (read_size == size)
                {
                    cb(error);
                    return;
                }
                aio_handler_.SetReadCallback(
                    std::bind(&TcpConnector::OnRead, this));
                aio_handler_.EnableRead();
                break;
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
    if (!is_connected_)
    {
        cb(ErrorType::NotConnected);
        return;
    }
    std::size_t write_size = 0;
    if (write_list_.empty())
    {
        while (true)
        {
            auto error = sock_.Write(data, size, &write_size);
            if (!error)
            {
                if (write_size == size)
                {
                    cb(error);
                    return;
                }
                aio_handler_.SetWriteCallback(
                    std::bind(&TcpConnector::OnWrite, this));
                aio_handler_.EnableWrite();
                break;
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
    if (connect_cb_)
    {
        auto callback = connect_cb_;
        connect_cb_ = nullptr;
        callback(Error());
    }
}

void TcpConnector::OnRead()
{
    if (!is_connected_)
    {
        MZX_FATAL("not connect but recv read event");
        return;
    }
    while (!read_list_.empty())
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
                if (info.read_size == info.size)
                {
                    (info.callback)(error);
                    read_list_.pop_front();
                    break;
                }
                return;
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
    }
    if (read_list_.empty())
    {
        // aio_handler_.EnableRead(false);
    }
}

void TcpConnector::OnWrite()
{
    if (!is_connected_)
    {
        MZX_FATAL("not connect buy recv write event");
        return;
    }
    while (!write_list_.empty())
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
                if (info.write_size == info.size)
                {
                    (info.callback)(error);
                    write_list_.pop_front();
                    break;
                }
                return;
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
    }
    if (write_list_.empty())
    {
        aio_handler_.EnableWrite(false);
    }
}

void TcpConnector::OnClose(const Error &error)
{
    aio_handler_.EnableAll(false);
    if (!is_connected_)
    {
        if (connect_cb_)
        {
            auto callback = connect_cb_;
            connect_cb_ = nullptr;
            callback(error);
        }
        else
        {
            MZX_FATAL("not connected and not connecting but recv close event");
        }
    }
    else
    {
        MZX_FATAL_IF(connect_cb_, "is connected buy connecting not null");
        is_connected_ = false;
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
        sock_.Close();
    }
}

} // namespace mzx
