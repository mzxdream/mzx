#include <mzx/event/net/m_tcp_connector.h>
#include <mzx/util/m_socket_opts.h>

MTcpConnector::MTcpConnector()
    :fd_(-1)
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
}
