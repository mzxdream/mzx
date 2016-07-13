#include <mzx/event/m_tcp_listener.h>
#include <mzx/util/m_socket_opts.h>

MTcpListener::MTcpListener()
    :p_event_loop_(nullptr)
    ,fd_(-1)
    ,close_on_free_(true)
    ,acceptable_(true)
{
}

MTcpListener::~MTcpListener()
{
    Clear();
}

MError MTcpListener::Init(MEventLoop *p_event_loop, int fd, bool close_on_free)
{
    if (!p_event_loop || fd < 0)
    {
        return MError::InvalidArgument;
    }
    p_event_loop_ = p_event_loop;
    fd_ = fd;
    close_on_free_ = close_on_free;
    acceptable_ = true;
    return MError::No;
}

void MTcpListener::Clear()
{
    if (fd_ >= 0)
    {
        if (p_event_loop_)
        {
            p_event_loop_->DelIOEvent(fd_);
        }
        if (close_on_free_)
        {
            MSocketOpts::Destroy(fd_);
        }
    }
    accept_buffers_.clear();
}

MError MTcpListener::AsyncAccept(const std::function<void (int, std::string, unsigned, MError)> &cb)
{
    if (!p_event_loop_ || !cb)
    {
        return MError::InvalidArgument;
    }
    if (!acceptable_)
    {
        accept_buffers_.push_back(cb);
        return MError::No;
    }
    MError err = MSocketOpts::Accept(fd_, accept_fd_, accept_ip_, accept_port_);
    if (err != MError::No)
    {
        if (err != MError::INTR && err != MError::Again)
        {
            this->MIOEventBase::DisableAllEvent();
            cb_(accepted_fd_, accepted_ip_, accepted_port_, err);
        }
        return;
    }
}

MError MTcpListener::StopAccept()
{
}

void MTcpListener::_OnCallback(unsigned events)
{
    MError err = MError::No;
    int count = once_count_;
    while (count < 0 || count-- > 0)
    {
        if (!cb_)//callback can stop listen
        {
            return;
        }
        err = MSocketOpts::Accept(fd_, accepted_fd_, accepted_ip_, accepted_port_);
        if (err != MError::No)
        {
            if (err != MError::INTR && err != MError::Again)
            {
                this->MIOEventBase::DisableAllEvent();
                cb_(accepted_fd_, accepted_ip_, accepted_port_, err);
            }
            return;
        }
        cb_(accepted_fd_, accepted_ip_, accepted_port_, err);
    }
}
