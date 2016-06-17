#ifndef _M_AFTER_IDLE_H_
#define _M_AFTER_IDLE_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_base.h>
#include <functional>

class MAfterIdle
{
public:
    MAfterIdle();
    ~MAfterIdle();
    MAfterIdle(const MAfterIdle &) = delete;
    MAfterIdle& operator=(const MAfterIdle &) = delete;
public:
    MError Init(MEventLoop *p_event_loop);
    void Clear();
    MError Start(const std::function<void ()> &cb, int repeated = 0);
    MError Stop();
public:
    void OnCallback();
private:
    MAfterEventBase event_base_;
    std::function<void ()> cb_;
    int repeated_;
};

#endif
