#include <mzx/net/m_tcp_listen.h>
#include <mzx/net/m_socket_opts.h>

MTcpListen::MTcpListen()
    :fd_(-1)
    ,once_count_(0)
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
    this->MIOEventBase::Clear();
}

MError MTcpListen::Start(int once_count, const std::function<void (int, MError)> &cb)
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
    return this->MIOEventBase::DisableAllEvent();
}

void MTcpListen::_OnCallback(unsigned events)
{
    int count = once_count_;
    while (count < 0 || count-- > 0)
    {
    }
}
