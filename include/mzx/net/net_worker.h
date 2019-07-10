#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

#include <asio.hpp>
#include <functional>
#include <mutex>

namespace mzx
{

class NetWorker final
{
public:
    using PostOutFunc = std::function<void()>;

    explicit NetWorker();
    ~NetWorker();
    NetWorker(const NetWorker &) = delete;
    NetWorker &operator=(const NetWorker &);

public:
    void Start();
    void Stop();
    void Join();
    void PostOut(PostOutFunc func);
    void ProcessOut();

private:
    void Run();

private:
    asio::io_service io_service_;
    asio::io_service::work io_service_work_;
    mzx::Thread work_thread_;
    std::list<PostOutFunc> post_out_list_;
    std::mutex post_out_mutex_;
};

} // namespace mzx

#endif
