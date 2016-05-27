#include <mzx/net/m_tcp_stream.h>
#include <mzx/net/m_socket_opts.h>

MTcpStream::MTcpStream()
    :fd_(-1)
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
    readable_ = true;
    writable_ = true;
    read_buffers_.clear();
    write_buffers_.clear();
    return this->MIOEventBase::Init(p_event_loop, fd);
}

void MTcpStream::Clear()
{
    this->MIOEventBase::Clear();
}

MError MTcpStream::Read(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb)
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
        return ret.second;
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

void MTcpStream::StopRead()
{
    read_buffers_.clear();
}

MError MTcpStream::Write(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb)
{
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

void MTcpStream::StopWrite()
{
    write_buffers_.clear();
}

void MTcpStream::OnError(MError err)
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
    std::function<void (std::size_t, MError)> write_cb;
    while ((it_write = write_buffers_.begin()) != write_buffers_.end())
    {
        write_cb = it_write->write_cb;
        write_buffers_.erase(it_write);
        if (write_cb)
        {
            write_cb(err);
        }
    }
}

void MTcpStream::_OnCallback(unsigned events)
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
            if (ret.second != MError::INTR && )

        }
    }
    if (events & MIOEVENT_OUT)
    {

    }


    if (events & MIOEVENT_IN)
    {
        std::list<MTcpReadBuffer>::iterator it_read = read_buffers_.begin();
        while (true)
        {
            it_read != read_buffers_.end()
            if (it_read->len == it_read->read_len)
            {
                if (it_read->read_cb)
                {
                    (it_read->read_cb)(it_read->len, MError::No);
                }
                continue;
            }
            std::pair<int, MError> ret = MSocketOpts::Recv(fd_, it_read->p_buf+it_read->read_len, it_read->len-it_read->read_len);
            if (ret.second == MError::No)
            {

            }
            if (it_read->read_cb)
            {
                (it_read->read_cb)(0, err);
            }
            it_read = read_buffers_.erase(it_read);
        }
    }
    else if (events & MIOEVENT_OUT)
    {
        std::list<MTcpWriteBuffer>::iterator it_write = write_buffers_.begin();
        while (it_write != write_buffers_.end())
        {
            if (it_write->write_cb)
            {
                (it_write->write_cb)(err);
            }
            it_write = write_buffers_.erase(it_write);
        }
    }
}
