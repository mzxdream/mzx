#include <mzx/event/m_tcp_event.h>
#include <mzx/net/m_socket_opts.h>

MTcpEvent::MTcpEvent()
{
}

MTcpEvent::~MTcpEvent()
{
    Clear();
}

MError MTcpEvent::Init(MEventLoop *p_event_loop, int fd)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    MError err = this->MIOEventBase::Init(p_event_loop, fd);
    if (err != MError::No)
    {
        return err;
    }
    return MError::No;
}

MError MTcpEvent::Clear()
{
    this->MIOEventBase::Clear();
}

MError MTcpEvent::StartConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb)
{
}
