#ifndef _M_EVENT_BASE_H_
#define _M_EVENT_BASE_H_

#include <mzx/util/m_errno.h>
#include <sys/epoll.h>
#include <mzx/util/m_type_define.h>
#include <map>
#include <list>
#include <functional>

class MEventLoop;

class MTimerEventBase
{
    typedef std::multimap<int64_t, MTimerEventBase*>::iterator MTimerEventLocation;
public:
    MTimerEventBase();
    ~MTimerEventBase();
    MTimerEventBase(const MTimerEventBase &) = delete;
    MTimerEventBase& operator=(const MTimerEventBase &) = delete;
public:
    bool IsActived() const;

    MEventLoop* GetEventLoop();

    MError Init(MEventLoop *p_event_loop);
    void Clear();
    MError EnableEvent(int64_t start_time, const std::function<void ()> &cb);
    MError DisableEvent();
private:
    friend class MEventLoop;
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    bool actived_;
    MTimerEventLocation location_;
};

class MBeforeEventBase
{
public:
    MBeforeEventBase();
    ~MBeforeEventBase();
    MBeforeEventBase(const MBeforeEventBase &) = delete;
    MBeforeEventBase& operator=(const MBeforeEventBase &) = delete;
public:
    bool IsActived() const;

    MError Init(MEventLoop *p_event_loop);
    void Clear();
    MError EnableEvent(const std::function<void ()> &cb);
    MError DisableEvent();
private:
    friend class MEventLoop;
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    MBeforeEventBase *p_prev_;
    MBeforeEventBase *p_next_;
};

class MAfterEventBase
{
    typedef std::list<MAfterEventBase*>::iterator MAfterEventLocation;
public:
    MAfterEventBase();
    ~MAfterEventBase();
    MAfterEventBase(const MAfterEventBase &) = delete;
    MAfterEventBase& operator=(const MAfterEventBase &) = delete;
public:
    bool IsActived() const;

    MError Init(MEventLoop *p_event_loop);
    void Clear();
    MError EnableEvent(const std::function<void ()> &cb);
    MError DisableEvent();
private:
    friend class MEventLoop;
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    MAfterEventBase *p_prev_;
    MAfterEventBase *p_next_;
};

#endif
