#include <mzx/logger.h>
#include <mzx/net/net_manager.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetManager::NetManager(std::size_t worker_count)
{
    MZX_CHECK(worker_count > 0);
    for (std::size_t i = 0; i < worker_count; ++i)
    {
        worker_list_.emplace_back(new NetWorker());
    }
}

NetManager::~NetManager()
{
    for (std::size_t i = 0; i < worker_list_.size(); ++i)
    {
        delete worker_list_[i];
    }
    for (std::size_t i = 0; i < acceptor_list_.size(); ++i)
    {
        delete acceptor_list_[i];
    }
    for (std::size_t i = 0; i < peer_connector_list_.size(); ++i)
    {
        delete peer_connector_list_[i];
    }
    for (std::size_t i = 0; i < connector_list_.size(); ++i)
    {
        delete connector_list_[i];
    }
}

NetID NetManager::AddAcceptor(const NetAcceptorConfInfo &info)
{
    return 0;
}

} // namespace mzx
