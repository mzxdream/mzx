#ifndef __MZX_NET_MANAGER_H__
#define __MZX_NET_MANAGER_H__

#include <atomic>
#include <vector>

#include <mzx/data_structure/list.h>
#include <mzx/net/net_define.h>

namespace mzx
{

class NetWorker;

class NetManager final
{
public:
    NetManager(std::size_t worker_count);
    ~NetManager();
    NetManager(const NetManager &) = delete;
    NetManager &operator=(const NetManager &) = delete;

public:
    NetID AddAcceptor(const NetAcceptorConfInfo &info);
    void RemoveAcceptor(NetID id);
    NetID AddPeerConnector(const NetPeerConnectorConfInfo &info);
    void RemovePeerConnector(NetID id);

    void Update(std::size_t count);

private:
    std::vector<NetWorker *> worker_list_;
    std::atomic<std::size_t> worker_next_use_{0};
    std::size_t worker_next_update_{0};
    std::vector<NetAcceptor *> acceptor_list_;
    std::vector<NetPeerConnector *> peer_connector_list_;
    std::vector<NetConnector *> connector_list_;
    List connector_free_list_;
};

} // namespace mzx

#endif
