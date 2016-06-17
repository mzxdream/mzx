#include <mzx/event/m_event_loop.h>
#include <mzx/event/m_event_base.h>

MIOEventBase::MIOEventBase()
    :p_event_loop_(nullptr)
    ,fd_(-1)
    ,events_(0)
    ,actived_(false)
{
}

MIOEventBase::~MIOEventBase()
{
    Clear();
}

int MIOEventBase::GetFD() const
{
    return fd_;
}

unsigned MIOEventBase::GetEvents() const
{
    return events_;
}

bool MIOEventBase::IsActived() const
{
    return actived_;
}

MError MIOEventBase::Init(MEventLoop *p_event_loop, int fd)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    if (fd < 0)
    {
        return MError::Invalid;
    }
    p_event_loop_ = p_event_loop;
    fd_ = fd;
    events_ = 0;
    actived_ = false;
    cb_ = nullptr;
    return MError::No;
}

void MIOEventBase::Clear()
{
    DisableAllEvent();
}

MError MIOEventBase::EnableEvent(unsigned events, const std::function<void (unsigned)> &cb)
{
    cb_ = cb;
    return p_event_loop_->AddIOEvent(events, this);
}

MError MIOEventBase::DisableEvent(unsigned events)
{
    return p_event_loop_->DelIOEvent(events, this);
}

MError MIOEventBase::DisableAllEvent()
{
    return DisableEvent(static_cast<unsigned>(-1));
}

void MIOEventBase::SetEvents(unsigned events)
{
    events_ = events;
}

void MIOEventBase::SetActived(bool actived)
{
    actived_ = actived;
}

void MIOEventBase::OnCallback(unsigned events)
{
    if (cb_)
    {
        cb_(events);
    }
}

MTimerEventBase::MTimerEventBase()
    :p_event_loop_(nullptr)
    ,actived_(false)
{
}

MTimerEventBase::~MTimerEventBase()
{
    Clear();
}

bool MTimerEventBase::IsActived() const
{
    return actived_;
}

MEventLoop* MTimerEventBase::GetEventLoop()
{
    return p_event_loop_;
}

MError MTimerEventBase::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    p_event_loop_ = p_event_loop;
    cb_ = nullptr;
    actived_ = false;
    return MError::No;
}

void MTimerEventBase::Clear()
{
    DisableEvent();
}

MError MTimerEventBase::EnableEvent(int64_t start_time, const std::function<void ()> &cb)
{
    cb_ = cb;
    return p_event_loop_->AddTimerEvent(start_time, this);
}

MError MTimerEventBase::DisableEvent()
{
    return p_event_loop_->DelTimerEvent(this);
}

void MTimerEventBase::SetLocation(MTimerEventLocation location)
{
    location_ = location;
}

MTimerEventLocation MTimerEventBase::GetLocation() const
{
    return location_;
}

void MTimerEventBase::SetActived(bool actived)
{
    actived_ = actived;
}

void MTimerEventBase::OnCallback()
{
    if (cb_)
    {
        cb_();
    }
}

MBeforeEventBase::MBeforeEventBase()
    :p_event_loop_(nullptr)
    ,actived_(false)
{
}

MBeforeEventBase::~MBeforeEventBase()
{
    Clear();
}

bool MBeforeEventBase::IsActived() const
{
    return actived_;
}

MError MBeforeEventBase::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    p_event_loop_ = p_event_loop;
    cb_ = cb;
    actived_ = false;
    return MError::No;
}

void MBeforeEventBase::Clear()
{
    DisableEvent();
}

MError MBeforeEventBase::EnableEvent(const std::function<void ()> &cb)
{
    cb_ = cb;
    return p_event_loop_->AddBeforeEvent(this);
}

MError MBeforeEventBase::DisableEvent()
{
    return p_event_loop_->DelBeforeEvent(this);
}

void MBeforeEventBase::SetLocation(MBeforeEventLocation location)
{
    location_ = location;
}

MBeforeEventLocation MBeforeEventBase::GetLocation() const
{
    return location_;
}

void MBeforeEventBase::SetActived(bool actived)
{
    actived_ = actived;
}

void MBeforeEventBase::OnCallback()
{
    if (cb_)
    {
        cb_();
    }
}

MAfterEventBase::MAfterEventBase()
    :p_event_loop_(nullptr)
    ,actived_(false)
{
}

MAfterEventBase::~MAfterEventBase()
{
    Clear();
}

bool MAfterEventBase::IsActived() const
{
    return actived_;
}

MError MAfterEventBase::Init(MEventLoop *p_event_loop)
{
    if (p_event_loop)
    {
        return MError::Invalid;
    }
    p_event_loop_ = p_event_loop;
    cb_ = nullptr;
    actived_ = false;
    return MError::No;
}

void MAfterEventBase::Clear()
{
    DisableEvent();
}

MError MAfterEventBase::EnableEvent(const std::function<void ()> &cb)
{
    cb_ = cb;
    return p_event_loop_->AddAfterEvent(this);
}

MError MAfterEventBase::DisableEvent()
{
    return p_event_loop_->DelAfterEvent(this);
}

void MAfterEventBase::SetActived(bool actived)
{
    actived_ = actived;
}

void MAfterEventBase::SetLocation(MAfterEventLocation location)
{
    location_ = location;
}

MAfterEventLocation MAfterEventBase::GetLocation() const
{
    return location_;
}

void MAfterEventBase::OnCallback()
{
    if (cb_)
    {
        cb_();
    }
}
