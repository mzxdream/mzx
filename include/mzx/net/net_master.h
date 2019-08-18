#ifndef __MZX_NET_MASTER_H__
#define __MZX_NET_MASTER_H__

#include <vector>

namespace mzx
{

class NetWorker;

class NetMaster
{
public:
    NetMaster();
    ~NetMaster();
    NetMaster(const NetMaster &) = delete;
    NetMaster &operator=(const NetMaster &) = delete;

public:
    bool Init();
    void Uninit();

    bool AddAcceptor();
    bool AddPeerConnector();

private:
    std::vector<NetWorker *> worker_list_;
};

} // namespace mzx

#endif
