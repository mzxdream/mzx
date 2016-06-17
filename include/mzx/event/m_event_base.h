#ifndef _M_EVENT_BASE_H_
#define _M_EVENT_BASE_H_

#include <mzx/util/m_errno.h>
#include <sys/epoll.h>
#include <mzx/util/m_type_define.h>
#include <map>
#include <list>
#include <functional>

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

class MEventLoop;

class MIOEventBase
{
public:
    MIOEventBase();
    ~MIOEventBase();
    MIOEventBase(const MIOEventBase &) = delete;
    MIOEventBase& operator=(const MIOEventBase &) = delete;
public:
    int GetFD() const;
    unsigned GetEvents() const;
    bool IsActived() const;

    MError Init(MEventLoop *p_event_loop, int fd);
    void Clear();
    MError EnableEvent(unsigned events, const std::function<void (unsigned)> &cb);
    MError DisableEvent(unsigned events);
    MError DisableAllEvent();
private:
    friend class MEventLoop;
    void SetEvents(unsigned events);
    void SetActived(bool actived);
    void OnCallback(unsigned events);
private:
    MEventLoop *p_event_loop_;
    int fd_;
    unsigned events_;
    std::function<void (unsigned)> cb_;
    bool actived_;
};

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
    void SetLocation(MTimerEventLocation location);
    MTimerEventLocation GetLocation() const;
    void SetActived(bool actived);
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    bool actived_;
    MTimerEventLocation location_;
};

class MBeforeEventBase
{
    typedef std::list<MBeforeEventBase*>::iterator MBeforeEventLocation;
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
    void SetLocation(MBeforeEventLocation location);
    MBeforeEventLocation GetLocation() const;
    void SetActived(bool actived);
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    bool actived_;
    MBeforeEventLocation location_;
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
    void SetActived(bool actived);
    void SetLocation(MAfterEventLocation location);
    MAfterEventLocation GetLocation() const;
    void OnCallback();
private:
    MEventLoop *p_event_loop_;
    std::function<void ()> cb_;
    bool actived_;
    MAfterEventLocation location_;
};

#endif
