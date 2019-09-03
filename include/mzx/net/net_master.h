#ifndef __MZX_NET_MASTER_H__
#define __MZX_NET_MASTER_H__

#include <functional>
#include <string>
#include <vector>

#include <mzx/error.h>
#include <mzx/net/net_define.h>
#include <mzx/net/net_socket.h>

namespace mzx
{

class NetWorker;

struct NetAcceptorConf final
{
    std::string bind_ip;
    short bind_port{0};
    std::function<void(NetConnectionID, const char *, std::size_t)> recv_cb;
    std::function<void(NetConnectionID, const std::string &, short)> connect_cb;
    std::function<void(NetConnectionID, Error)> disconnect_cb;
};

struct NetAcceptorInfo final
{
    NetConnectionID id{kNetConnectionIDInvalid};
    NetSocketID sock{kNetSocketIDInvalid};
    NetAcceptorConf conf;
};

struct NetPeerConnectorConf final
{
    std::string remote_ip;
    short remote_port{0};
    std::function<void(NetConnectionID, const char *, std::size_t)> recv_cb;
    std::function<void(NetConnectionID, const std::string &, short)> connect_cb;
    std::function<void(NetConnectionID, Error)> disconnect_cb;
};

struct NetPeerConnectorInfo final
{
    NetConnectionID id{kNetConnectionIDInvalid};
    std::size_t worker_index{0};
    NetPeerConnectorConf conf;
};

class NetMaster final
{
public:
    NetMaster();
    ~NetMaster();
    NetMaster(const NetMaster &) = delete;
    NetMaster &operator=(const NetMaster &) = delete;

public:
    bool Init(const NetConf &conf);
    void Uninit();
    void Update();

    NetConnectionID AddAcceptor(const NetAcceptorConf &conf);
    NetConnectionID AddPeerConnector(const NetPeerConnectorConf &conf);
    bool Send(NetConnectionID id, const char *data, std::size_t size);
    bool Disconnect(NetConnectionID id);

private:
    std::vector<NetWorker *> worker_list_;
    std::vector<NetAcceptorInfo *> acceptor_list_;
    std::vector<NetPeerConnectorInfo *> peer_connector_list_;
    NetConf conf_;
};

} // namespace mzx

#endif
