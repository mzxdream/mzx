#ifndef __MZX_AIO_SERVER_H__
#define __MZX_AIO_SERVER_H__

#include <functional>
#include <list>
#include <mutex>

#include <mzx/aio/aio_handler.h>
#include <mzx/thread.h>

namespace mzx
{

class AIOServer final
{
    friend AIOHandler;

public:
    using ExecFunc = std::function<void()>;

    explicit AIOServer();
    virtual ~AIOServer();
    AIOServer(const AIOServer &) = delete;
    AIOServer &operator=(const AIOServer &) = delete;

    bool Start();
    bool Stop();
    bool Join();
    bool CanExecImmediately() const;
    void Exec(ExecFunc func);
    void Post(ExecFunc func);

private:
    void Wakeup();
    void OnWakeup();
    void Run();

private:
    int epoll_fd_{-1};
    int wakeup_fd_{-1};
    AIOHandler *wakeup_handler_{nullptr};
    std::mutex exec_queue_mtx_;
    std::list<ExecFunc> exec_queue_;
    Thread thread_;
    volatile bool stop_flag_{false};
};

} // namespace mzx

#endif
