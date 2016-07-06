#include <mzx/event/net/m_tcp_connector.h>
#include <mzx/util/m_socket_opts.h>
#include <cstring>

MTcpConnector::MTcpConnector()
    :p_event_loop_(nullptr)
    ,fd_(-1)
    ,close_on_free_(true)
    ,readable_(true)
    ,writable_(true)
{
}

MTcpConnector::~MTcpConnector()
{
    Clear();
}

MError MTcpConnector::Init(MEventLoop *p_event_loop, int fd, bool close_on_free)
{
    if (!p_event_loop || fd < 0)
    {
        return MError::InvalidArgument;
    }
    p_event_loop_ = p_event_loop;
    fd_ = fd;
    close_on_free_ = close_on_free;
    readable_ = true;
    writable_ = true;
    return MError::No;
}

void MTcpConnector::Clear()
{
    if (fd_ >= 0)
    {
        if (p_event_loop_)
        {
            p_event_loop_->DelIOEvent(fd_);
        }
        if (close_on_free_)
        {
            MSocketOpts::Destroy(fd_);
        }
    }
    read_buffers_.clear();
    write_buffers_.clear();
}

MError MTcpConnector::AsyncConnect(const std::string &ip, unsigned port
    , const std::function<void (MError)> &cb)
{
    if (!p_event_loop_)
    {
        return MError::InvalidArgument;
    }
    MError err = MError::No;
    while ((err = MSocketOpts::Connect(fd_, ip, port))
        == MError::InterruptedSystemCall)
    {
    }
    if (err != MError::No && err != MError::InProgress)
    {
        return err;
    }
    err = p_event_loop_->AddIOEvent(fd_, MIOEVENT_OUT
        , std::bind(&MTcpConnector::OnConnectCallback, this));
    return err;
}

MError MTcpConnector::AsyncRead(char *p_buf, std::size_t len, std::size_t min_len
    , const std::function<void (std::size_t, MError)> &read_cb)
{
    if (!p_event_loop_ || fd < 0)
    {
        return MError::InvalidArgument;
    }
    if (!readable_)
    {
        MTcpReadBuffer buf;
        buf.p_buf = p_buf;
        buf.len = len;
        buf.min_len = min_len;
        buf.read_len = 0;
        buf.read_cb = read_cb;
        read_buffers_.push_back(buf);
        return MError::No;
    }
    if (len == 0)
    {
        if (read_cb)
        {
            read_cb(0, MError::No);
        }
        return MError::No;
    }
    std::pair<int, MError> ret = MSocketOpts::Recv(fd_, p_buf, len);
    std::size_t read_len = 0;
    if (ret.second == MError::No)
    {
        read_len = static_cast<std::size_t>(ret.first);
        if (read_len == 0)
        {
            OnError(MError::ConnectReset);
            return MError::ConnectReset;
        }
        if (read_len == len || read_len >= min_len)
        {
            if (read_cb)
            {
                read_cb(read_len, MError::No);
            }
            return MError::No;
        }
    }
    else if (ret.second != MError::InterruptedSystemCall
        && ret.second != MError::TryAgain)
    {
        OnError(ret.second);
        return ret.second;
    }
    MError err = p_event_loop_->AddIOEvent(fd_, MIOEVENT_IN|MIOEVENT_LT
        , std::bind(&MTcpConnector::OnStreamCallback, this));
    if (err != MError::No)
    {
        return err;
    }
    readable_ = false;
    MTcpReadBuffer buf;
    buf.p_buf = p_buf;
    buf.len = len;
    buf.min_len = min_len;
    buf.read_len = read_len;
    buf.read_cb = read_cb;
    read_buffers_.push_back(buf);
    return MError::No;
}

MError MTcpConnector::AsyncWrite(const char *p_buf, std::size_t len
    , std::size_t min_len, const std::function<void (std::size_t, MError)> &write_cb)
{
    if (!p_event_loop_ || fd < 0)
    {
        return MError::Invalid;
    }

    if (!writable_)
    {
        MTcpWriteBuffer buf;
        buf.p_buf = p_buf;
        buf.len = len;
        buf.min_len = min_len;
        buf.write_len = 0;
        buf.write_cb = write_cb;
        write_buffers_.push_back(buf);
        return MError::No;
    }
    if (len == 0)
    {
        if (write_cb)
        {
            write_cb(0, MError::No);
        }
        return MError::No;
    }
    std::pair<int, MError> ret = MSocketOpts::Send(fd_, p_buf, len);
    std::size_t write_len = 0;
    if (ret.second == MError::No)
    {
        write_len = static_cast<std::size_t>(ret.first);
        if (write_len == len || write_len >= min_len)
        {
            if (write_cb)
            {
                write_cb(write_len, MError::No);
            }
            return MError::No;
        }
    }
    else if (ret.second != MError::InterruptedSystemCall
        && ret.second != MError::TryAgain)
    {
        OnError(ret.second);
        return ret.second;
    }
    MError err = p_event_loop_->AddIOEvent(fd_, MIOEVENT_OUT|MIOEVENT_LT
        , std::bind(&MTcpConnector::OnStreamCallback, this));
    if (err != MError::No)
    {
        return err;
    }
    writable_ = false;
    MTcpWriteBuffer buf;
    buf.p_buf = p_buf;
    buf.len = len;
    buf.min_len = min_len;
    buf.write_len = write_len;
    buf.write_cb = write_cb;
    write_buffers_.push_back(buf);
    return MError::No;
}

void MTcpConnector::ClearRead()
{
    read_buffers_.clear();
}

void MTcpConnector::ClearWrite()
{
    write_buffers_.clear();
}

void MTcpConnector::OnError(MError err)
{
    for (auto it = read_buffers_.begin(); it != read_buffers_.end(); )
    {
        auto read_cb = it->read_cb;
        std::size_t read_len = it->read_len;
        read_buffers_.erase(it);
        if (read_cb)
        {
            read_cb(read_len, err);
        }
        it = read_buffers_.begin();
    }

    for (auto it = write_buffers_.begin(); it != write_buffers_.end(); )
    {
        auto write_cb = it->write_cb;
        std::size_t write_len = it->write_len;
        write_buffers_.erase(it);
        if (write_cb)
        {
            write_cb(write_len, err);
        }
        it = write_buffers_.begin();
    }
    p_event_loop_->DelIOEvent(fd_);
}

void MTcpConnector::OnConnectCallback(unsigned events)
{
    p_event_loop_->DelIOEvent(fd_);
    if (!cb_)
    {
        return;
    }
    if (events & (MIOEVENT_ERR|MIOEVENT_HUP))
    {
        cb_(MError::Unknown);
    }
    else if (events & MIOEVENT_RDHUP)
    {
        cb_(MError::ConnectReset);
    }
    else if (events & MIOEVENT_OUT)
    {
        cb_(MError::No);
    }
}

void MTcpConnector::OnStreamCallback(unsigned events)
{
    if (events & (MIOEVENT_HUP|MIOEVENT_ERR))
    {
        OnError(MError::Unknown);
        return;
    }
    if (events & MIOEVENT_RDHUP)
    {
        OnError(MError::Disconnect);
        return;
    }
    if (events & MIOEVENT_IN)
    {
        std::list<MTcpReadBuffer>::iterator it_read;
        std::function<void (std::size_t, MError)> read_cb;
        std::size_t read_len;
        std::pair<int, MError> ret;
        while (true)
        {
            it_read = read_buffers_.begin();
            if (it_read == read_buffers_.end())
            {
                p_event_loop_->DelIOEvent(MIOEVENT_IN);
                readable_ = true;
                break;
            }
            if (it_read->read_len == it_read->len
                || it_read->read_len >= it_read->min_len)
            {
                read_len = it_read->read_len;
                read_cb = it_read->read_cb;
                read_buffers_.erase(it_read);
                if (read_cb)
                {
                    read_cb(read_len, MError::No);
                }
                continue;
            }
            ret = MSocketOpts::Recv(fd_, it_read->p_buf+it_read->read_len, it_read->len-it_read->read_len);
            if (ret.second == MError::No)
            {
                read_len = static_cast<std::size_t>(ret.first);
                if (read_len == 0)
                {
                    OnError(MError::Disconnect);
                    return;
                }
                it_read->read_len += read_len;
                if (it_read->read_len == it_read->len)
                {
                    read_len = it_read->read_len;
                    read_cb = it_read->read_cb;
                    read_buffers_.erase(it_read);
                    if (read_cb)
                    {
                        read_cb(read_len, MError::No);
                    }
                    continue;
                }
                if (it_read->read_len >= it_read->min_len)
                {
                    read_len = it_read->read_len;
                    read_cb = it_read->read_cb;
                    read_buffers_.erase(it_read);
                    if (read_cb)
                    {
                        read_cb(read_len, MError::No);
                    }
                }
            }
            if (ret.second != MError::INTR && ret.second != MError::Again)
            {
                OnError(ret.second);
                return;
            }
            break;
        }
    }
    if (events & MIOEVENT_OUT)
    {
        std::list<MTcpWriteBuffer>::iterator it_write;
        std::function<void (MError)> write_cb;
        std::size_t write_len;
        std::pair<int, MError> ret;
        while (true)
        {
            it_write = write_buffers_.begin();
            if (it_write == write_buffers_.end())
            {
                p_event_loop_->DelIOEvent(MIOEVENT_OUT);
                writable_ = true;
                break;
            }
            if (it_write->write_len == it_write->len
                || it_write->write_len >= it_write->min_len)
            {
                write_len = it_write->write_len;
                write_cb = it_write->write_cb;
                write_buffers_.erase(it_write);
                if (write_cb)
                {
                    write_cb(write_len, MError::No);
                }
                continue;
            }
            ret = MSocketOpts::Send(fd_, it_write->p_buf+it_write->write_len, it_write->len-it_write->write_len);
            if (ret.second == MError::No)
            {
                write_len = static_cast<std::size_t>(ret.first);
                it_write->write_len += write_len;
                if (it_write->write_len == it_write->len)
                {
                    write_len = it_write->write_len;
                    write_cb = it_write->write_cb;
                    write_buffers_.erase(it_write);
                    if (write_cb)
                    {
                        write_cb(write_len, MError::No);
                    }
                    continue;
                }
                if (it_write->write_len >= it_write->min_len)
                {
                    write_len = it_write->write_len;
                    write_cb = it_write->write_cb;
                    write_buffers_.erase(it_write);
                    if (write_cb)
                    {
                        write_cb(write_len, MError::No);
                    }
                }
            }
            if (ret.second != MError::INTR && ret.second != MError::Again)
            {
                OnError(ret.second);
                return;
            }
            break;
        }
    }
}
