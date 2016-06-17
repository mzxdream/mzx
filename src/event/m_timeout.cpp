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
    MError err = event_base_.Init(p_event_loop);
    if (err != MError::No)
    {
        return err;
    }
    return MError::No;
}

void MTimeout::Clear()
{
    event_base_.Clear();
}

MError MTimeout::Start(const std::function<void ()> &cb, int timeout, int repeated)
{
    if (!cb || timeout <= 0)
    {
        return MError::Invalid;
    }
    MError err = event_base_.DisableEvent();
    if (err != MError::No)
    {
        return err;
    }
    cb_ = cb;
    timeout_ = timeout;
    repeated_ = repeated;
    return event_base_.EnableEvent(event_base_.GetEventLoop()->GetTime() + timeout_, std::bind(&MTimeout::OnCallback, this));
}

MError MTimeout::Stop()
{
    return event_base_.DisableEvent();
}

void MTimeout::OnCallback()
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
        event_base_.EnableEvent(event_base_.GetEventLoop()->GetTime() + timeout_, std::bind(&MTimeout::OnCallback, this));
    }
}
