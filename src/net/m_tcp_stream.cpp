#include <mzx/net/m_tcp_stream.h>
#include <mzx/net/m_socket_opts.h>

MTcpStream::MTcpStream()
    :fd_(-1)
    ,read_enable_(true)
    ,write_enable_(true)
    ,readable_(true)
    ,writable_(true)
{
}

MTcpStream::~MTcpStream()
{
    Clear();
}

MError MTcpStream::Init(MEventLoop *p_event_loop, int fd)
{
    if (!p_event_loop || fd < 0)
    {
        return MError::Invalid;
    }
    fd_ = fd;
    read_enable_ = true;
    write_enable_ = true;
    readable_ = true;
    writable_ = true;
    return this->MIOEventBase::Init(p_event_loop, fd);
}

void MTcpStream::Clear()
{
    this->MIOEventBase::Clear();
}

MError MTcpStream::EnableRead(bool enable)
{
    if (enable == read_enable_)
    {
        return MError::No;
    }
    if (enable)
    {
        if (!readable_)
        {
            MError err = this->MIOEventBase::EnableEvent(MIOEVENT_IN|MIOEVENT_ET);
            if (err != MError::No)
            {
                return err;
            }
        }
    }
    else
    {
        read_buffers_.clear();
    }
    read_enable_ = enable;
    return MError::No;
}

MError MTcpStream::EnableWrite(bool enable)
{
    if (enable == write_enable_)
    {
        return MError::No;
    }
    if (enable)
    {
        if (!writable_)
        {
            MError err = this->MIOEventBase::EnableEvent(MIOEVENT_OUT|MIOEVENT_ET);
            if (err != MError::No)
            {
                return err;
            }
        }
    }
    else
    {
        write_buffers_.clear();
    }
    write_enable_ = enable;
    return MError::No;
}

MError MTcpStream::AsyncRead(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb)
{
    if (!read_cb)
    {
        return MError::Invalid;
    }
    if (!read_enable_)
    {
        return MError::No;
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
        read_cb(len, MError::No);
        return MError::No;
    }
    std::pair<int, MError> ret = MSocketOpts::Recv(fd_, p_buf, len);
    std::size_t read_len = 0;
    if (ret.second == MError::No)
    {
        read_len = static_cast<size_t>(ret.first);
        if (read_len == len || read_len >= min_len)
        {
            read_cb(read_len, MError::No);
            return MError::No;
        }
    }
    else if (ret.second != MError::INTR && ret.second != MError::Again)
    {
        return MError::Unknown;
    }
    MError err = this->MIOEventBase::EnableEvent(MIOEVENT_IN|MIOEVENT_ET);
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

MError MTcpStream::AsyncWrite(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb)
{
    if (!write_enable_)
    {
        return MError::No;
    }
    if (!writable_)
    {
        MTcpWriteBuffer buf;
        buf.p_buf = p_buf;
        buf.len = len;
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
        write_len = static_cast<size_t>(ret.first);
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
        return MError::Unknown;
    }
    MError err = this->MIOEventBase::EnableEvent(MIOEVENT_OUT|MIOEVENT_ET);
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

void MTcpStream::_OnCallback(unsigned events)
{
    if (events & (MIOEVENT_RDHUP|MIOEVENT_HUP|MIOEVENT_ERR))
    {
        MError err = MError::Unknown;
        if (events & MIOEVENT_RDHUP)
        {
            err = MError::Disconnect;
        }
        std::list<MTcpReadBuffer>::iterator it_read = read_buffers_.begin();
        while (it_read != read_buffers_.end())
        {
            if (it_read->read_cb)
            {
                (it_read->read_cb)(0, err);
            }
            it_read = read_buffers_.erase(it_read);
        }
        std::list<MTcpWriteBuffer>::iterator it_write = write_buffers_.begin();
        while (it_write != write_buffers_.end())
        {
            if (it_write->write_cb)
            {
                (it_write->write_cb)(err);
            }
            it_write = write_buffers_.erase(it_write);
        }
        return;
    }
}
