#ifndef __MZX_AIO_SERVER_H__
#define __MZX_AIO_SERVER_H__

#include <functional>
#include <list>
#include <mutex>
#include <mzx/data_structure/list.h>

#include <mzx/aio/aio_operation.h>
#include <mzx/thread.h>

namespace mzx
{

class AIOServer;

class AIOHandler
{
    friend AIOServer;

private:
    explicit AIOHandler(AIOServer &aio_server, int fd);
    ~AIOHandler();
    AIOHandler(const AIOHandler &) = delete;
    AIOHandler &operator=(const AIOHandler &) = delete;

public:
    void StartRead();
    void StartWrite();

private:
    AIOServer &aio_server_;
    int fd_{-1};
    int registered_events_{0};
    std::list<AIOOperation *> read_list_;
    std::list<AIOOperation *> write_list_;
    std::mutex mutex_;
    mzx::ListNode list_link_;
};

class AIOServer final
{
public:
    explicit AIOServer();
    virtual ~AIOServer();
    AIOServer(const AIOServer &) = delete;
    AIOServer &operator=(const AIOServer &) = delete;

public:
    bool Start();
    void Stop();
    bool Join();

    AIOHandler *RegisterHandler(int fd, Error *error = nullptr);
    void DeregisterHandler(AIOHandler *handler);

    void Post(std::function<void()> func);

private:
    bool CanExecImmediately() const;
    void Wakeup();
    void OnWakeup();
    void Run();

private:
    int epoll_fd_{-1};
    int wakeup_fd_{-1};
    AIOHandler *wakeup_handler_{nullptr};
    List handler_list_;
    std::mutex handler_mutex_;
    std::list<AIOOperation *> complete_list_;
    std::mutex complete_mutex_;
    Thread thread_;
    volatile bool stop_flag_{false};
};

} // namespace mzx

#endif
