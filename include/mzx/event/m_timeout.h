#ifndef _M_TIMEOUT_H_
#define _M_TIMEOUT_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

class MTimeout
{
public:
    MTimeout();
    ~MTimeout();
    MTimeout(const MTimeout &) = delete;
    MTimeout& operator=(const MTimeout &) = delete;
public:
    MError Init(MEventLoop *p_event_loop);
    void Clear();
    MError Start(const std::function<void ()> &cb, int timeout, int repeated = 0);
    MError Stop();
public:
    void OnCallback();
private:
    MTimerEventBase event_base_;
    std::function<void ()> cb_;
    int timeout_;
    int repeated_;
};

#endif
