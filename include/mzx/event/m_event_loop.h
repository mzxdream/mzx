#ifndef _M_EVENT_LOOP_H_
#define _M_EVENT_LOOP_H_

#include <map>
#include <vector>
#include <list>
#include <mzx/util/m_errno.h>
#include <mzx/util/m_type_define.h>
#include <sys/epoll.h>
#include <mzx/event/m_event_base.h>

#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif

#define MIOEVENT_IN      EPOLLIN
#define MIOEVENT_OUT     EPOLLOUT
#define MIOEVENT_RDHUP   EPOLLRDHUP
#define MIOEVENT_LT      0
#define MIOEVENT_ET      EPOLLET
#define MIOEVENT_ERR     EPOLLERR
#define MIOEVENT_HUP     EPOLLHUP

class MEventLoop
{
public:
    MEventLoop();
    ~MEventLoop();
    MEventLoop(const MEventLoop &) = delete;
    MEventLoop& operator=(const MEventLoop &) = delete;
public:
    MError Init();
    void Clear();

    int64_t GetTime() const;
    void UpdateTime();

    MError AddIOEvent(int fd, unsigned events, const std::function<void (unsigned)> &cb);
    MError DelIOEvent(int fd, unsigned events = MIOEVENT_IN|MIOEVENT_OUT|MIOEVENT_RDHUP);

    MError AddTimerEvent(int64_t start_time, MTimerEventBase *p_event);
    MError DelTimerEvent(MTimerEventBase *p_event);

    MError AddBeforeEvent(MBeforeEventBase *p_event);
    MError DelBeforeEvent(MBeforeEventBase *p_event);

    MError AddAfterEvent(MAfterEventBase *p_event);
    MError DelAfterEvent(MAfterEventBase *p_event);

    MError Interrupt();
    MError DispatchEvent();
    MError DispatchEventOnce(int timeout = -1);
private:
    MError AddInterrupt();
    MError DispatchIOEvent(int timeout);
    MError DispatchTimerEvent();
    MError DispatchBeforeEvent();
    MError DispatchAfterEvent();
private:
    int epoll_fd_;
    int64_t cur_time_;
    int interrupter_[2];

    std::vector<epoll_event> io_events_;
    std::map<int, std::pair<unsigned, std::function<void (unsigned)> > > io_handlers_;
    std::multimap<int64_t, MTimerEventBase*> timer_events_;
    MBeforeEventBase before_events_;
    MAfterEventBase after_events_;
};

#endif
