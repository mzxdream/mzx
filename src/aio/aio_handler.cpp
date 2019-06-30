#include <cstring>
#include <sys/epoll.h>

#include <mzx/aio/aio_handler.h>
#include <mzx/logger.h>

namespace mzx
{

AIOHandler::AIOHandler(int fd, int efd)
    : fd_(fd)
    , efd_(efd)
{
    MZX_CHECK(fd_ >= 0);
    MZX_CHECK(efd_ >= 0);
}

AIOHandler::~AIOHandler()
{
    EnableAll(false);
}

void AIOHandler::SetReadCallback(ReadCallback cb)
{
    read_cb_ = cb;
}

void AIOHandler::SetWriteCallback(WriteCallback cb)
{
    write_cb_ = cb;
}

void AIOHandler::SetCloseCallback(CloseCallback cb)
{
    close_cb_ = cb;
}

static bool EpollCtl(int efd, int fd, int prev_events, int events, void *ptr)
{
    int op = EPOLL_CTL_MOD;
    if (prev_events & (EPOLLIN | EPOLLOUT))
    {
        if (!(events & (EPOLLIN | EPOLLOUT)))
        {
            op = EPOLL_CTL_DEL;
        }
    }
    else
    {
        if (!(events & (EPOLLIN | EPOLLOUT)))
        {
            MZX_WARN("epoll:", efd, " ctl:", fd, " events:", events,
                     " not has in or out event");
            return false;
        }
        op = EPOLL_CTL_ADD;
    }
    epoll_event ee;
    ee.events = events;
    ee.data.ptr = ptr;
    int ret = epoll_ctl(efd, op, fd, &ee);
    if (ret < 0)
    {
        MZX_ERR("epoll:", efd, " ctl:", fd, " events:", events,
                " failed ret:", ret);
        return false;
    }
    return true;
}

void AIOHandler::EnableRead(bool enable)
{
    if (enable)
    {
        if (events_ & EPOLLIN)
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, events_ | EPOLLIN | EPOLLET, this))
        {
            events_ |= (EPOLLIN | EPOLLET);
        }
    }
    else
    {
        if (!(events_ & EPOLLIN))
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, events_ & ~EPOLLIN, this))
        {
            events_ &= ~EPOLLIN;
        }
    }
}

void AIOHandler::EnableWrite(bool enable)
{
    if (enable)
    {
        if (events_ & EPOLLOUT)
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, events_ | EPOLLOUT | EPOLLET, this))
        {
            events_ |= (EPOLLOUT | EPOLLET);
        }
    }
    else
    {
        if (!(events_ & EPOLLOUT))
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, events_ & ~EPOLLOUT, this))
        {
            events_ &= ~EPOLLOUT;
        }
    }
}

void AIOHandler::EnableAll(bool enable)
{
    if (enable)
    {
        if ((events_ & EPOLLIN) && (events_ & EPOLLOUT))
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, events_ | EPOLLIN | EPOLLOUT | EPOLLET,
                     this))
        {
            events_ |= (EPOLLIN | EPOLLOUT | EPOLLET);
        }
    }
    else
    {
        if (!(events_ & EPOLLIN) && !(events_ & EPOLLOUT))
        {
            return;
        }
        if (EpollCtl(efd_, fd_, events_, 0, this))
        {
            events_ = 0;
        }
    }
}

void AIOHandler::HandleEvent(int events)
{
    if (events & EPOLLERR)
    {
        MZX_ERR("epoll:", efd_, " fd:", fd_, " error");
        if (close_cb_)
        {
            close_cb_(Error(ErrorType::Unknown));
            return;
        }
    }
    if (events & (EPOLLHUP | EPOLLRDHUP))
    {
        if (close_cb_)
        {
            close_cb_(Error());
            return;
        }
    }
    if (events & EPOLLIN)
    {
        if (read_cb_)
        {
            read_cb_();
        }
    }
    if (events & EPOLLOUT)
    {
        if (write_cb_)
        {
            write_cb_();
        }
    }
}

} // namespace mzx
