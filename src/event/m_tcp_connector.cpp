#include <mzx/event/net/m_tcp_connector.h>
#include <mzx/util/m_socket_opts.h>
#include <cstring>

MTcpConnector::MTcpConnector()
    :close_on_free_(true)
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
        return MError::Invalid;
    }
    MError err = event_base_.Init(p_event_loop, fd);
    if (err != MError::No)
    {
        return err;
    }
    close_on_free_ = close_on_free;
    readable_ = true;
    writable_ = true;
    return MError::No;
}

void MTcpConnector::Clear()
{
    int fd = event_base_.GetFD();
    event_base_.Clear();
    if (close_on_free_ && fd >= 0)
    {
        MSocketOpts::Destroy(fd);
    }
    read_buffers_.clear();
    for (auto it = write_buffers_.begin(); it != write_buffers_.end();)
    {
        free(it->p_buf);
        it = write_buffers_.erase(it);
    }
}

MError MTcpConnector::AsyncConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb)
{
    MError err = this->MIOEventBase::DisableAllEvent();
    if (err != MError::No)
    {
        return err;
    }
    err = this->MIOEventBase::EnableEvent(MIOEVENT_IN|MIOEVENT_LT);
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    while ((err = MSocketOpts::Connect(fd_, ip, port)) == MError::INTR)
    {
    }
    if (err == MError::InProgress)
    {
        return MError::No;
    }
    return err;

}

MError MTcpConnector::AsyncRead(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb)
{
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
            OnError(MError::Disconnect);
            return MError::Disconnect;
        }
        else if (read_len == len || read_len >= min_len)
        {
            if (read_cb)
            {
                read_cb(read_len, MError::No);
            }
            return MError::No;
        }
    }
    else if (ret.second != MError::INTR && ret.second != MError::Again)
    {
        OnError(ret.second);
        return ret.second;
    }
    MError err = this->MIOEventBase::EnableEvent(MIOEVENT_IN|MIOEVENT_ET, std::bind(&MTcpConnector::OnStreamCallback, this));
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

MError MTcpConnector::AsyncWrite(const char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (MError)> &write_cb)
{
    if (!writable_)
    {
        MTcpWriteBuffer buf;
        buf.p_buf = static_cast<char*>(malloc(sizeof(char)*len));
        memcpy(buf.p_buf, p_buf, len);
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
            write_cb(MError::No);
        }
        return MError::No;
    }
    std::pair<int, MError> ret = MSocketOpts::Send(fd_, p_buf, len);
    std::size_t write_len = 0;
    if (ret.second == MError::No)
    {
        write_len = static_cast<std::size_t>(ret.first);
        if (write_len == len)
        {
            if (write_cb)
            {
                write_cb(MError::No);
            }
            return MError::No;
        }
    }
    else if (ret.second != MError::INTR && ret.second != MError::Again)
    {
        return ret.second;
    }
    MError err = this->MIOEventBase::EnableEvent(MIOEVENT_OUT|MIOEVENT_ET, std::bind(&MTcpConnector::OnStreamCallback, this));
    if (err != MError::No)
    {
        return err;
    }
    writable_ = false;
    MTcpWriteBuffer buf;
    buf.p_buf = p_buf;
    buf.len = len;
    buf.write_len = write_len;
    buf.write_cb = write_cb;
    write_buffers_.push_back(buf);
    return MError::No;
}

MError MTcpConnector::ClearRead()
{
    read_buffers_.clear();
    return this->MIOEventBase::DisableEvent(MIOEVENT_IN);
}

MError MTcpConnector::ClearWrite()
{
    for (auto it = write_buffers_.begin(); it != write_buffers_.end(); )
    {
        free(it->p_buf);
        it = write_buffers_.erase(it);
    }
    return this->MIOEventBase::DisableEvent(MIOEVENT_OUT);
}

MError MTcpConnector::ClearReadWrite()
{
    read_buffers_.clear();
    for (auto it = write_buffers_.begin(); it != write_buffers_.end(); )
    {
        free(it->p_buf);
        it = write_buffers_.erase(it);
    }
    return this->MIOEventBase::DisableEvent(MIOEVENT_IN|MIOEVENT_OUT);
}

void MTcpConnector::OnError(MError err)
{
    std::list<MTcpReadBuffer>::iterator it_read;
    std::function<void (std::size_t, MError)> read_cb;
    while ((it_read = read_buffers_.begin()) != read_buffers_.end())
    {
        read_cb = it_read->read_cb;
        read_buffers_.erase(it_read);
        if (read_cb)
        {
            read_cb(0, err);
        }
    }

    std::list<MTcpWriteBuffer>::iterator it_write;
    std::function<void (MError)> write_cb;
    while ((it_write = write_buffers_.begin()) != write_buffers_.end())
    {
        write_cb = it_write->write_cb;
        write_buffers_.erase(it_write);
        if (write_cb)
        {
            write_cb(err);
        }
    }
    this->MIOEventBase::DisableAllEvent();
}

void MTcpConnector::OnConnectCallback(unsigned events)
{
    this->MIOEventBase::DisableAllEvent();
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
        cb_(MError::Disconnect);
    }
    else if (events & MIOEVENT_IN)
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
            if ((it_read = read_buffers_.begin()) == read_buffers_.end())
            {
                MError err = this->MIOEventBase::DisableEvent(MIOEVENT_IN);
                if (err != MError::No)
                {
                    OnError(err);
                    return;
                }
                readable_ = true;
                break;
            }
            if (it_read->read_len == it_read->len || it_read->read_len >= it_read->min_len)
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
                break;
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
            if ((it_write = write_buffers_.begin()) == write_buffers_.end())
            {
                MError err = this->MIOEventBase::DisableEvent(MIOEVENT_OUT);
                if (err != MError::No)
                {
                    OnError(err);
                    return;
                }
                writable_ = true;
                break;
            }
            if (it_write->write_len == it_write->len)
            {
                write_len = it_write->write_len;
                write_cb = it_write->write_cb;
                write_buffers_.erase(it_write);
                if (write_cb)
                {
                    write_cb(MError::No);
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
                        write_cb(MError::No);
                    }
                    continue;
                }
                break;
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
