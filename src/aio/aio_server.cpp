#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <mzx/aio/aio_server.h>
#include <mzx/logger.h>

namespace mzx
{

AIOServer::AIOServer()
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    MZX_CHECK(epoll_fd_ >= 0);
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    MZX_CHECK(wakeup_fd_ >= 0);
    wakeup_handler_ = new AIOHandler(wakeup_fd_, epoll_fd_);
    wakeup_handler_->EnableRead();
}

AIOServer::~AIOServer()
{
    Stop();
    if (wakeup_handler_)
    {
        delete wakeup_handler_;
    }
    close(wakeup_fd_);
    close(epoll_fd_);
}

void AIOServer::Stop()
{
    stop_flag_ = true;
    Wakeup();
    Join();
}

void AIOServer::Exec(ExecFunc func)
{
    MZX_CHECK(func != nullptr);
    if (IsCurrentThread())
    {
        func();
    }
    else
    {
        Post(std::move(func));
    }
}

void AIOServer::Post(ExecFunc func)
{
    std::lock_guard<std::mutex> lock(exec_queue_mtx_);
    if (exec_queue_.empty())
    {
        Wakeup();
    }
    exec_queue_.push_back(func);
}

void AIOServer::Wakeup()
{
    uint64_t tmp = 1;
    write(wakeup_fd_, &tmp, sizeof(tmp));
}

void AIOServer::OnWakeup()
{
    uint64_t tmp = 0;
    read(wakeup_fd_, &tmp, sizeof(tmp));
}

void AIOServer::HandleExecQueue()
{
    MZX_CHECK(IsCurrentThread());
    while (!exec_queue_.empty())
    {
        auto exec_func = std::move(exec_queue_.front());
        exec_queue_.pop_front();
        exec_func();
    }
}

void AIOServer::_Run()
{
}

} // namespace mzx
