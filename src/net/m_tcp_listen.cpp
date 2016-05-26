#include <mzx/net/m_tcp_listen.h>
#include <mzx/net/m_socket_opts.h>

MTcpListen::MTcpListen()
    :fd_(-1)
    ,once_count_(0)
    ,cb_(nullptr)
{
}

MTcpListen::~MTcpListen()
{
    Clear();
}

MError MTcpListen::Init(MEventLoop *p_event_loop, int fd)
{
    if (!p_event_loop || fd < 0)
    {
        return MError::Invalid;
    }
    MError err = this->MIOEventBase::Init(p_event_loop, fd);
    if (err != MError::No)
    {
        return err;
    }
    fd_ = fd;
    return MError::No;
}

void MTcpListen::Clear()
{
    cb_ = nullptr;
    this->MIOEventBase::Clear();
}

MError MTcpListen::Start(int once_count, const std::function<void (int, std::string, unsigned, MError)> &cb)
{
    if (once_count == 0 || !cb)
    {
        return MError::Invalid;
    }
    MError err = this->MIOEventBase::DisableAllEvent();
    if (err != MError::No)
    {
        return err;
    }
    once_count_ = once_count;
    cb_ = cb;
    return this->MIOEventBase::EnableEvent(MIOEVENT_IN|MIOEVENT_LEVEL);
}

MError MTcpListen::Stop()
{
    cb_ = nullptr;
    return this->MIOEventBase::DisableAllEvent();
}

void MTcpListen::_OnCallback(unsigned events)
{
    MError err = MError::No;
    int count = once_count_;
    while (count < 0 || count-- > 0)
    {
        if (!cb_)//callback can stop listen
        {
            return;
        }
        err = MSocketOpts::Accept(fd_, accepted_fd_, accepted_ip_, accepted_port_);
        if (err != MError::No)
        {
            if (err != MError::INTR && err != MError::Again)
            {
                this->MIOEventBase::DisableAllEvent();
                cb_(accepted_fd_, accepted_ip_, accepted_port_, err);
            }
            return;
        }
        cb_(accepted_fd_, accepted_ip_, accepted_port_, err);
    }
}
