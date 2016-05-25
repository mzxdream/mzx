#include <mzx/event/m_before_idle.h>

MBeforeIdle::MBeforeIdle()
    :repeated_(0)
{
}

MBeforeIdle::~MBeforeIdle()
{
    Clear();
}

MError MBeforeIdle::Init(MEventLoop *p_event_loop)
{
    if (!p_event_loop)
    {
        return MError::Invalid;
    }
    return this->MBeforeEventBase::Init(p_event_loop);
}

void MBeforeIdle::Clear()
{
    this->MBeforeEventBase::Clear();
}

MError MBeforeIdle::Start(const std::function<void ()> &cb, int repeated)
{
    if (!cb)
    {
        return MError::Invalid;
    }
    MError err = this->MBeforeEventBase::DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    repeated_ = repeated;
    return this->MBeforeEventBase::EnableEvent();
}

MError MBeforeIdle::Stop()
{
    return this->MBeforeEventBase::DisableEvent();
}

void MBeforeIdle::_OnCallback()
{
    cb_();
    if (repeated_ != 0)
    {
        if (repeated_ > 0)
        {
            --repeated_;
        }
        this->MBeforeEventBase::EnableEvent();
    }
}
