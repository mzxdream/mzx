#include <mzx/net/m_tcp_stream.h>
#include <mzx/net/m_socket_opts.h>

MTcpStream::MTcpStream()
    :fd_(-1)
    ,read_cache_{nullptr, 0, 0, 0}
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

MError MTcpStream::Init(MEventLoop *p_event_loop, int fd, std::size_t read_cache_len)
{
    if (!p_event_loop || fd < 0)
    {
        return MError::Invalid;
    }

    fd_ = fd;
    if (read_cache_len > 0)
    {
        read_cache_.p_buf = new char[read_cache_len+1];
        if (!read_cache_.p_buf)
        {
            return MError::NoBufs;
        }
        read_cache_.len = read_cache_len + 1;
    }
    else
    {
        read_cache_.p_buf = nullptr;
        read_cache_.len = 0;
    }
    read_cache_.pos_start = 0;
    read_cache_.pos_end = 0;
    read_enable_ = true;
    write_enable_ = true;
    readable_ = true;
    writable_ = true;
    return this->MIOEventBase::Init(p_event_loop, fd);
}

void MTcpStream::Clear()
{
    this->MIOEventBase::Clear();
    if (read_cache_.p_buf)
    {
        delete read_cache_.p_buf;
        read_cache_.p_buf = nullptr;
    }
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
    write_enable_ = enable;
    return MError::No;
}

MError MTcpStream::AsyncRead(char *p_buf, std::size_t min_len, std::size_t max_len, const std::function<void (std::size_t, MError)> &read_cb)
{
    return MError::No;
}

MError MTcpStream::AsyncWrite(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb)
{
    return MError::No;
}

void MTcpStream::_OnCallback(unsigned events)
{
}
