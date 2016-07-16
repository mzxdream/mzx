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
        return MError::InvalidArgument;
    }
    return event_base_.Init(p_event_loop);
}

void MAfterIdle::Clear()
{
    event_base_.Clear();
}

MError MAfterIdle::Start(const std::function<void ()> &cb, int repeated)
{
    if (!cb)
    {
        return MError::InvalidArgument;
    }
    MError err = event_base_.DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    repeated_ = repeated;
    return event_base_.EnableEvent(std::bind(&MAfterIdle::OnCallback, this));
}

MError MAfterIdle::Stop()
{
    return event_base_.DisableEvent();
}

void MAfterIdle::OnCallback()
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
        event_base_.EnableEvent(std::bind(&MAfterIdle::OnCallback, this));
    }
}
