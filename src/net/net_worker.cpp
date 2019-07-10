#include <mzx/net/net_worker.h>
#include <mzz/logger.h>

namespace mzx
{

NetWorker::NetWorker()
    : io_service_work_(io_service_)
    , work_thread_(std::bind(&NetWorker::Run, this))
{
}

NetWorker::~NetWorker()
{
    MZX_CHECK(!work_thread_.Joinable());
    ProcessOut();
}

void NetWorker::Start()
{
    work_thread_.Start();
}

void NetWorker::Stop()
{
    io_service_.stop();
}

void NetWorker::Join()
{
    work_thread_.Join();
}

void NetWorker::PostOut(PostOutFunc func)
{
    MZX_CHECK(func != nullptr);
    std::lock_guard<std::mutex> lock(post_out_mutex_);
    post_out_list_.push_back(std::move(func));
}

void NetWorker::ProcessOut()
{
    std::list<PostOutFunc> out_list;
    {
        std::lock_guard<std::mutex> lock(post_out_mutex_);
        out_list.swap(post_out_list_);
    }
    while (!out_list.empty())
    {
        (out_list.front())();
        out_list.pop_front();
    }
}

void NetWorker::Run()
{
    io_service_.run();
}

} // namespace mzx
