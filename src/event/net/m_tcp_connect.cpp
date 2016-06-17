#include <mzx/net/m_tcp_connect.h>
#include <mzx/net/m_socket_opts.h>

MTcpConnect::MTcpConnect()
    :fd_(-1)
{
}

MTcpConnect::~MTcpConnect()
{
    Clear();
}

MError MTcpConnect::Init(MEventLoop *p_event_loop, int fd)
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

void MTcpConnect::Clear()
{
    this->MIOEventBase::Clear();
}

MError MTcpConnect::Start(const std::string &ip, unsigned port, const std::function<void (MError)> &cb)
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

MError MTcpConnect::Stop()
{
    return this->MIOEventBase::DisableAllEvent();
}

void MTcpConnect::_OnCallback(unsigned events)
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
