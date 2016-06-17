#ifndef _M_TCP_CONNECT_H_
#define _M_TCP_CONNECT_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

class MTcpConnect
{
public:
    MTcpConnect();
    virtual ~MTcpConnect();
    MTcpConnect(const MTcpConnect &) = delete;
    MTcpConnect& operator=(const MTcpConnect &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd);
    void Clear();
    MError Start(const std::string &ip, unsigned port, const std::function<void (MError)> &cb);
    MError Stop();
public:
    void OnCallback(unsigned events);
private:
    MIOEventBase event_base_;
    std::function<void (MError)> cb_;
};

#endif
