#include <sys/epoll.h>
#include <sys/eventfd.h>

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
}

AIOServer::~AIOServer()
{
}

void AIOServer::Exec(ExecFunc func)
{
}

void AIOServer::Post(ExecFunc func)
{
}

void AIOServer::_Run()
{
}

} // namespace mzx
