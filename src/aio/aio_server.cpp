#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <mzx/aio/aio_server.h>
#include <mzx/logger.h>
#include <mzx/macro_util.h>

namespace mzx
{

AIOServer::AIOServer()
    : thread_(std::bind(&AIOServer::Run, this))
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    MZX_CHECK(epoll_fd_ >= 0);
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    MZX_CHECK(wakeup_fd_ >= 0);
    wakeup_handler_ = new AIOHandler(*this, wakeup_fd_);
    wakeup_handler_->EnableRead();
}

AIOServer::~AIOServer()
{
    MZX_CHECK(thread_.Joinable());
    if (wakeup_handler_)
    {
        delete wakeup_handler_;
    }
    close(wakeup_fd_);
    close(epoll_fd_);
}

bool AIOServer::Start()
{
    return thread_.Start();
}

void AIOServer::Stop()
{
    stop_flag_ = true;
    Wakeup();
    return true;
}

bool AIOServer::Join()
{
    return thread_.Join();
}

bool AIOServer::CanExecImmediately() const
{
    return !thread_.Joinable() || thread_.IsCurrentThread();
}

void AIOServer::Exec(ExecFunc func, bool forcePost)
{
    MZX_CHECK(func != nullptr);
    if (!forcePost && CanExecImmediately())
    {
        func();
    }
    else
    {
        std::lock_guard<std::mutex> lock(exec_queue_mtx_);
        if (exec_queue_.empty())
        {
            Wakeup();
        }
        exec_queue_.push_back(std::move(func));
    }
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

void AIOServer::Run()
{
    epoll_event events[1024];
    std::list<ExecFunc> exec_list;
    stop_flag_ = false;
    while (!stop_flag_)
    {
        auto nevents =
            epoll_wait(epoll_fd_, events, MZX_ARRAY_SIZE(events), -1);
        if (nevents == -1)
        {
            if (errno != EINTR)
            {
                MZX_FATAL("epoll wait failed err:", errno);
            }
        }
        else
        {
            for (int i = 0; i < nevents; ++i)
            {
                auto &ee = events[i];
                auto *aio_handler = static_cast<AIOHandler *>(ee.data.ptr);
                aio_handler->HandleEvent(ee.events);
            }
        }
        {
            std::lock_guard<std::mutex> lock(exec_queue_mtx_);
            exec_list.swap(exec_queue_);
        }
        for (auto &exec_func : exec_list)
        {
            exec_func();
        }
        exec_list.clear();
    }
}

} // namespace mzx
