#include <mzx/event/m_event_loop.h>
#include <fcntl.h>
#include <mzx/util/m_logger.h>
#include <mzx/util/m_time.h>
#include <unistd.h>

MEventLoop::MEventLoop()
    :epoll_fd_(-1)
    ,cur_time_(0)
    ,interrupter_{-1, -1}
{
}

MEventLoop::~MEventLoop()
{
    Clear();
}

MError MEventLoop::AddInterrupt()
{
    if (pipe(interrupter_) != 0)
    {
        MLOG(MGetDefaultLogger(), MERR, "create pipe failed, errno:", errno);
        return MError::Unknown;
    }
    fcntl(interrupter_[0], F_SETFL, O_NONBLOCK);
    fcntl(interrupter_[1], F_SETFL, O_NONBLOCK);
    epoll_event ee;
    ee.events = EPOLLIN | EPOLLERR | EPOLLET;
    ee.data.fd = interrupter_[0];
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, interrupter_[0], &ee) == -1)
    {
        MLOG(MGetDefaultLogger(), MERR, "epoll ctl failed, errno:", errno);
        return MError::Unknown;
    }
    int tmp = 1;
    write(interrupter_[1], &tmp, sizeof(tmp));
    return MError::No;
}

MError MEventLoop::Init()
{
    if ((epoll_fd_ = epoll_create1(EPOLL_CLOEXEC)) == -1)
    {
        MLOG(MGetDefaultLogger(), MERR, "create epoll failed, errno:", errno);
        return MError::Unknown;
    }
    MError err = AddInterrupt();
    if (err != MError::No)
    {
        return err;
    }
    UpdateTime();
    io_events_.resize(1024);
    before_events_.p_prev_ = &before_events_;
    before_events_.p_next_ = &before_events_;
    after_events_.p_prev_ = &after_events_;
    after_events_.p_next_ = &after_events_;
    return MError::No;
}

void MEventLoop::Clear()
{
    if (epoll_fd_ >= 0)
    {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    if (interrupter_[1] != interrupter_[0]
        && interrupter_[1] >= 0)
    {
        close(interrupter_[1]);
        interrupter_[1] = -1;
    }
    if (interrupter_[0] >= 0)
    {
        close(interrupter_[0]);
        interrupter_[0] = -1;
    }
    io_handlers_.clear();
    for (auto it = timer_events_.begin(); it != timer_events_.end(); )
    {
        if (it->second)
        {
            it->second->actived_ = false;
        }
        it = timer_events_.erase(it);
    }
    while (before_events_.p_next_ != &before_events_)
    {
        MBeforeEventBase *p_event = before_events_.p_next_;
        p_event->p_prev_->p_next_ = p_event->p_next_;
        p_event->p_next_->p_prev_ = p_event->p_prev_;
        p_event->p_prev_ = nullptr;
        p_event->p_next_ = nullptr;
    }
    while (after_events_.p_next_ != &after_events_)
    {
        MBeforeEventBase *p_event = after_events_.p_next_;
        p_event->p_prev_->p_next_ = p_event->p_next_;
        p_event->p_next_->p_prev_ = p_event->p_prev_;
        p_event->p_prev_ = nullptr;
        p_event->p_next_ = nullptr;
    }
}

int64_t MEventLoop::GetTime() const
{
    return cur_time_;
}

void MEventLoop::UpdateTime()
{
    cur_time_ = MTime::GetTime();
}

MError MEventLoop::AddIOEvent(int fd, unsigned events, const std::function<void (unsigned)> &cb)
{
    if (fd < 0)
    {
        MLOG(MGetDefaultLogger(), MERR, "fd is Invalid");
        return MError::Invalid;
    }
    auto it = io_handlers_.find(fd);
    if (it == io_handlers_.end())
    {
        if (!(events & (MIOEVENT_IN|MIOEVENT_OUT|MIOEVENT_RDHUP)))
        {
            MLOG(MGetDefaultLogger(), MERR, "events is not in out or rdhup");
            return MError::Invalid;
        }
        epoll_event ee;
        ee.data.fd = fd;
        ee.events = events;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ee) == -1)
        {
            MLOG(MGetDefaultLogger(), MERR, "epoll add failed errno:", errno);
            return MError::Unknown;
        }
        io_handlers_.insert(std::pair<int, std::pair<unsigned, std::function<void (unsigned)> > >(fd, ee.events, cb));
    }
    else
    {
        epoll_event ee;
        ee.data.fd = fd;
        ee.events = it->second.first | events;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ee) == -1)
        {
            MLOG(MGetDefaultLogger(), MERR, "epoll mod failed errno:", errno);
            return MError::Unknown;
        }
        it->second.first = ee.events;
        it->second.second = cb;
    }
    return MError::No;
}

MError MEventLoop::DelIOEvent(int fd, unsigned events)
{
    auto it = io_handlers_.find(fd);
    if (it != io_handlers_.end())
    {
        epoll_event ee;
        ee.data.fd = fd;
        ee.events = it->second.first & ~events;
        if (!(ee.events & (MIOEVENT_IN|MIOEVENT_OUT|MIOEVENT_RDHUP)))
        {
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ee) == -1)
            {
                MLOG(MGetDefaultLogger(), MERR, "epoll del failed errno:", errno);
                return MError::Unknown;
            }
            io_handlers_.erase(it);
        }
        else
        {
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ee) == -1)
            {
                MLOG(MGetDefaultLogger(), MERR, "epoll mod failed errno:", errno);
                return MError::Unknown;
            }
            it->second.first = ee.events;
        }
    }
    return MError::No;
}

MError MEventLoop::AddTimerEvent(int64_t start_time, MTimerEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (p_event->IsActived())
    {
        MError err = DelTimerEvent(p_event);
        if (err != MError::No)
        {
            return err;
        }
    }
    p_event->location_ = timer_events_.insert(std::make_pair(start_time, p_event));
    p_event->actived_ = true;
    return MError::No;
}

MError MEventLoop::DelTimerEvent(MTimerEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (!p_event->IsActived())
    {
        return MError::No;
    }
    timer_events_.erase(p_event->location_);
    p_event->actived_ = false;
    return MError::No;
}

MError MEventLoop::AddBeforeEvent(MBeforeEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (p_event->IsActived())
    {
        MError err = DelBeforeEvent(p_event);
        if (err != MError::No)
        {
            return err;
        }
    }
    p_event->p_next_ = &before_events_;
    p_event->p_prev_ = before_events_.p_prev_;
    p_event->p_prev_->p_next_ = p_event;
    before_events_.p_prev_ = p_event;
    return MError::No;
}

MError MEventLoop::DelBeforeEvent(MBeforeEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (!p_event->IsActived())
    {
        return MError::No;
    }
    p_event->p_prev_->p_next_ = p_event->p_next_;
    p_event->p_next_->p_prev_ = p_event->p_prev_;
    p_event->p_prev_ = nullptr;
    p_event->p_next_ = nullptr;
    return MError::No;
}

MError MEventLoop::AddAfterEvent(MAfterEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (p_event->IsActived())
    {
        return MError::No;
    }
    p_event->p_next_ = &after_events_;
    p_event->p_prev_ = after_events_.p_prev_;
    p_event->p_prev_->p_next_ = p_event;
    before_events_.p_prev_ = p_event;
    return MError::No;
}

MError MEventLoop::DelAfterEvent(MAfterEventBase *p_event)
{
    if (!p_event)
    {
        MLOG(MGetDefaultLogger(), MERR, "event is Invalid");
        return MError::Invalid;
    }
    if (!p_event->IsActived())
    {
        return MError::No;
    }
    p_event->p_prev_->p_next_ = p_event->p_next_;
    p_event->p_next_->p_prev_ = p_event->p_prev_;
    p_event->p_prev_ = nullptr;
    p_event->p_next_ = nullptr;
    return MError::No;
}

MError MEventLoop::Interrupt()
{
    epoll_event ee;
    ee.events = EPOLLIN | EPOLLERR | EPOLLET;
    ee.data.fd = interrupter_[0];
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, interrupter_[0], &ee) == -1)
    {
        MLOG(MGetDefaultLogger(), MERR, "epoll ctl failed errno:", errno);
        return MError::Unknown;
    }
    return MError::No;
}

MError MEventLoop::DispatchEvent()
{
    MError err = MError::No;
    while ((err = DispatchEventOnce()) == MError::No)
    {
    }
    return err;
}

MError MEventLoop::DispatchEventOnce(int timeout)
{
    MError err = DispatchBeforeEvent();
    if (err != MError::No)
    {
        MLOG(MGetDefaultLogger(), MERR, "DispatchBeforeEvent failed");
        return err;
    }
    err = DispatchIOEvent(timeout);
    if (err != MError::No)
    {
        MLOG(MGetDefaultLogger(), MERR, "DispatchIOEvent failed");
        return err;
    }
    UpdateTime();
    err = DispatchTimerEvent();
    if (err != MError::No)
    {
        MLOG(MGetDefaultLogger(), MERR, "DispatchTimerEvent failed");
        return err;
    }
    err = DispatchAfterEvent();
    if (err != MError::No)
    {
        MLOG(MGetDefaultLogger(), MERR, "DispatchAfterEvent failed");
        return err;
    }
    return MError::No;
}

MError MEventLoop::DispatchIOEvent(int timeout)
{
    if (!timer_events_.empty())
    {
        int left_time = std::max(0, static_cast<int>(timer_events_.begin()->first - cur_time_));
        if (timeout < 0)
        {
            timeout = left_time;
        }
        else
        {
            timeout = std::min(timeout, left_time);
        }
    }
    bool interrupt = false;
    int count = 48;
    int64_t start_time = cur_time_;
    do
    {
        int nevents = epoll_wait(epoll_fd_, &io_events_[0], io_events_.size(), timeout);
        if (nevents == -1)
        {
            if (errno != EINTR)
            {
                MLOG(MGetDefaultLogger(), MERR, "epoll wait failed errno:", errno);
                return MError::Unknown;
            }
        }
        else
        {
            for (int i = 0; i < nevents; ++i)
            {
                int fd = io_events_[i].data.fd;
                if (fd == interrupter_[0])
                {
                    interrupt = true;
                    continue;
                }
                auto it = io_handlers_.find(fd);
                if (it == io_handlers_.end())
                {
                    epoll_event ee;
                    ee.data.fd = fd;
                    ee.events = 0;
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ee);
                }
                else
                {
                    auto &cb = it->second.second;
                    if (cb)
                    {
                        cb(io_events_[i].events);
                    }
                }
            }
        }
        if (interrupt)
        {
            break;
        }
        if (nevents > 0)
        {
            if (static_cast<size_t>(nevents) == io_events_.size() && (--count > 0))
            {
                continue;
            }
            break;
        }
        UpdateTime();
        if (timeout > 0 && start_time >= cur_time_ - timeout)
        {
            break;
        }
    } while (true);
    return MError::No;
}

MError MEventLoop::DispatchTimerEvent()
{
    auto it = timer_events_.begin();
    while (it != timer_events_.end()
        && it->first <= cur_time_)
    {
        MTimerEventBase *p_event = it->second;
        timer_events_.erase(it);
        if (p_event)
        {
            p_event->actived_ = false;
            p_event->OnCallback();
        }
        it = timer_events_.begin();
    }
    return MError::No;
}

MError MEventLoop::DispatchBeforeEvent()
{
    if (before_events_.p_next_ == &before_events_)
    {
        return MError::No;
    }
    MBeforeEventBase events;
    events.p_prev_ = before_events_.p_prev_;
    events.p_prev_->p_next_ = &events;
    events.p_next_ = before_events_.p_next_;
    events.p_next_->p_prev_ = &events;
    before_events_.p_prev_ = &before_events_;
    before_events_.p_next_ = &before_events_;

    while (events.p_next_ != &events)
    {
        MBeforeEventBase *p_event = events.p_next_;
        p_event->p_prev_->p_next_ = p_event->p_next_;
        p_event->p_next_->p_prev_ = p_event->p_prev_;
        p_event->p_prev_ = nullptr;
        p_event->p_next_ = nullptr;
        p_event->OnCallback();
    }
    return MError::No;
}

MError MEventLoop::DispatchAfterEvent()
{
    if (after_events_.p_next_ == &after_events_)
    {
        return MError::No;
    }
    MAfterEventBase events;
    events.p_prev_ = after_events_.p_prev_;
    events.p_prev_->p_next_ = &events;
    events.p_next_ = after_events_.p_next_;
    events.p_next_->p_prev_ = &events;
    after_events_.p_prev_ = &after_events_;
    after_events_.p_next_ = &after_events_;

    while (events.p_next_ != &events)
    {
        MAfterEventBase *p_event = events.p_next_;
        p_event->p_prev_->p_next_ = p_event->p_next_;
        p_event->p_next_->p_prev_ = p_event->p_prev_;
        p_event->p_prev_ = nullptr;
        p_event->p_next_ = nullptr;
        p_event->OnCallback();
    }
    return MError::No;
}
