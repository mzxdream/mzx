#ifndef _M_TCP_CONNECTOR_H_
#define _M_TCP_CONNECTOR_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

class MTcpConnector
{
public:
    MTcpConnector();
    ~MTcpConnector();
    MTcpConnector(const MTcpConnector &) = delete;
    MTcpConnector& operator=(const MTcpConnector &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd, bool close_on_free);
    void Clear();

    MError StartConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb);
    MError StopConnect();

    MError StartRead(std::size_t max_len, const std::function<void (MError)> &cb);
    MError StopRead();

    MError Write();
    MError WriteSome();
public:
    void OnConnectCallback(unsigned events);
    void OnStreamCallback(unsigned events);
private:
    MIOEventBase event_base_;
    std::function<void (MError)> connect_cb_;
    std::function<void (MError)> read_cb_;
};

#endif
