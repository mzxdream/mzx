#ifndef _M_TCP_LISTEN_H_
#define _M_TCP_LISTEN_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

class MTcpListen
    :public MIOEventBase
{
public:
    MTcpListen();
    virtual ~MTcpListen();
    MTcpListen(const MTcpListen &) = delete;
    MTcpListen& operator=(const MTcpListen &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd);
    void Clear();
    MError Start(int once_count, const std::function<void (int, std::string, unsigned, MError)> &cb);
    MError Stop();
private:
    virtual void _OnCallback(unsigned events) override;
private:
    int fd_;
    int once_count_;
    std::function<void (int, std::string, unsigned, MError)> cb_;
    int accepted_fd_;
    std::string accepted_ip_;
    unsigned accepted_port_;
};

#endif
