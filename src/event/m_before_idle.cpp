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
    return event_base_.Init(p_event_loop);
}

void MBeforeIdle::Clear()
{
    event_base_.Clear();
}

MError MBeforeIdle::Start(const std::function<void ()> &cb, int repeated)
{
    if (!cb)
    {
        return MError::Invalid;
    }
    MError err = event_base_.DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    repeated_ = repeated;
    return event_base_.EnableEvent(std::bind(&MBeforeIdle::OnCallback, this));
}

MError MBeforeIdle::Stop()
{
    return event_base_.DisableEvent();
}

void MBeforeIdle::OnCallback()
{
    if (cb_)
    {
        cb_();
    }
    if (repeated_ != 0)
    {
        if (repeated_ > 0)
        {
            --repeated_;
        }
        event_base_.EnableEvent(std::bind(&MBeforeIdle::OnCallback, this));
    }
}
