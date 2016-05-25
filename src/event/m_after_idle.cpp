#include <mzx/event/m_after_idle.h>

MAfterIdle::MAfterIdle()
    :repeated_(0)
{
}

MAfterIdle::~MAfterIdle()
{
    Clear();
}

MError MAfterIdle::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    return this->MAfterEventBase::Init(p_event_loop);
}

void MAfterIdle::Clear()
{
    this->MAfterEventBase::Clear();
}

MError MAfterIdle::Start(const std::function<void ()> &cb, int repeated)
{
    if (!cb)
    {
        return MError::Invalid;
    }
    MError err = this->MAfterEventBase::DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    repeated_ = repeated;
    return this->MAfterEventBase::EnableEvent();
}

MError MAfterIdle::Stop()
{
    return this->MAfterEventBase::DisableEvent();
}

void MAfterIdle::_OnCallback()
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
