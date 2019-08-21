#ifndef __MZX_NET_ACCEPTOR_H__
#define __MZX_NET_ACCEPTOR_H__

#include <functional>

#include <mzx/logger.h>
#include <mzx/error.h>
#include <mzx/net/net_define.h>
#include <mzx/net/net_address.h>
#include <mzx/net/net_buffer.h>

namespace mzx
{

struct NetAcceptorConf final
{
    NetAddress addr;
    std::function<void(NetConnectionID, const char *, std::size_t)> recv_cb;
    std::function<void(NetConnectionID, const NetAddress &)> connect_cb;
    std::function<void(NetConnectionID, Error)> disconnect_cb;
};

class NetMaster;

class NetAcceptor final
{
public:
    NetAcceptor();
    ~NetAcceptor();
    NetAcceptor(const NetAcceptor &) = delete;
    NetAcceptor &operator=(const NetAcceptor &) = delete;

public:
    void SetID(NetConnectionID id)
    {
        id_ = id;
    }
    NetConnectionID ID() const
    {
        return id_;
    }

    bool Init(NetMaster *master, const NetAcceptorConf &conf);
    void Unint();

public:
    void OnRecv(NetConnectionID connection_id, NetBuffer *buffer);
    void OnConnected(NetConnectionID connection_id, const NetAddress &addr);
    void OnDisconnected(NetConnectionID connection_id, Error error);

private:
    NetConnectionID id_{kNetConnectionIDInvalid};
    int sock_{-1};
    std::function<void(NetConnectionID, const char *, std::size_t)> recv_cb_;
    std::function<void(NetConnectionID, const NetAddress &)> connect_cb_;
    std::function<void(NetConnectionID, Error)> disconnect_cb_;
};

} // namespace mzx

#endif
