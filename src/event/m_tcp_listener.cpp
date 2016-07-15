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
    int fd = -1;
    std::string ip;
    unsigned port = 0;
    MError err = MSocketOpts::Accept(fd_, fd, ip, port);
    if (err == MError::No)
    {
        cb(fd, ip, port, err);
        return err;
    }
    if (err != MError::InterruptedSystemCall && err != MError::TryAgain)
    {
        return err;
    }
    err = p_event_loop_->AddIOEvent(fd_, MIOEVENT_IN|MIOEVENT_LT, std::bind(&MTcpListener::OnAcceptCallback, this));
    if (err != MError::No)
    {
        return err;
    }
    acceptable_ = false;
    accept_buffers_.push_back(cb);
    return MError::No;
}

MError MTcpListener::StopAccept()
{
    accept_buffers_.clear();
    if (p_event_loop_)
    {
        return p_event_loop_->DelIOEvent(fd_);
    }
    return MError::No;
}

void MTcpListener::OnError(MError err)
{
    for (auto it = accept_buffers_.begin(); it != accept_buffers_.end(); )
    {
        auto accept_cb = *it;
        accept_buffers_.erase(it);
        if (accept_cb)
        {
            accept_cb(-1, "", 0, err);
        }
        it = accept_buffers_.begin();
    }
    p_event_loop_->DelIOEvent(fd_);
}

void MTcpListener::OnAcceptCallback(unsigned events)
{
    if (events & (MIOEVENT_HUP|MIOEVENT_ERR))
    {
        OnError(MError::Unknown);
        return;
    }
    if (events & MIOEVENT_RDHUP)
    {
        OnError(MError::ConnectReset);
        return;
    }
    if (events & MIOEVENT_IN)
    {
        while (true)
        {
            auto it_accept = accept_buffers_.begin();
            if (it_accept == accept_buffers_.end())
            {
                p_event_loop_->DelIOEvent(fd_, MIOEVENT_IN);
                acceptable_ = true;
                break;
            }
            int fd = -1;
            std::string ip;
            unsigned port = 0;
            MError err = MSocketOpts::Accept(fd_, fd, ip, port);
            if (err == MError::No)
            {
                cb(fd, ip, port, err);
                return err;
            }
            if (err != MError::InterruptedSystemCall && err != MError::TryAgain)
            {
                return err;
            }

            MError err = MSocketOpts::Accept();
        }
    }
}
