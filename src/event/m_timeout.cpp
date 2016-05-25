#include <mzx/event/m_timeout.h>

MTimeout::MTimeout()
    :p_event_loop_(nullptr)
    ,timeout_(0)
    ,repeated_(0)
{
}

MTimeout::~MTimeout()
{
    Clear();
}

MError MTimeout::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    MError err = this->MTimerEventBase::Init(p_event_loop);
    if (err != MError::No)
    {
        return err;
    }
    p_event_loop_ = p_event_loop;
    return MError::No;
}

void MTimeout::Clear()
{
    this->MTimerEventBase::Clear();
}

MError MTimeout::Start(const std::function<void ()> &cb, int timeout, int repeated)
{
    if (!cb || timeout <= 0)
    {
        return MError::Invalid;
    }
    MError err = this->MTimerEventBase::DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    timeout_ = timeout;
    repeated_ = repeated;
    err = this->MTimerEventBase::EnableEvent(p_event_loop_->GetTime() + timeout_);
    return err;
}

MError MTimeout::Stop()
{
    return this->MTimerEventBase::DisableEvent();
}

void MTimeout::_OnCallback()
{
    cb_();
    if (repeated_ != 0)
    {
        if (repeated_ > 0)
        {
            --repeated_;
        }
        this->MTimerEventBase::EnableEvent(p_event_loop_->GetTime() + timeout_);
    }
}
