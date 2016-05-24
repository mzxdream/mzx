#include <mzx/event/m_after_idle_event.h>

MAfterIdleEvent::MAfterIdleEvent()
    :repeated_(0)
{
}

MAfterIdleEvent::~MAfterIdleEvent()
{
    Clear();
}

MError MAfterIdleEvent::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    return this->MAfterEventBase::Init(p_event_loop);
}

void MAfterIdleEvent::Clear()
{
    this->MAfterEventBase::Clear();
}

MError MAfterIdleEvent::EnableEvent(const std::function<void ()> &cb, int repeated)
{
    if (!cb)
    {
        return MError::Invalid;
    }
    MError err = DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    repeated_ = repeated;
    return this->MAfterEventBase::EnableEvent();
}

MError MAfterIdleEvent::DisableEvent()
{
    return this->MAfterEventBase::DisableEvent();
}

void MAfterIdleEvent::_OnCallback()
{
    cb_();
    if (repeated_ != 0)
    {
        if (repeated_ > 0)
        {
            --repeated_;
        }
        this->MAfterEventBase::EnableEvent();
    }
}
