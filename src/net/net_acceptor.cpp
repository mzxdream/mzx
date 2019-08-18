#include <mzx/net/net_acceptor.h>
#include <mzx/net/net_master.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetAcceptor::NetAcceptor()
{
}

NetAcceptor::~NetAcceptor()
{
    Unint();
}

bool NetAcceptor::Init(NetMaster *master, const NetAcceptorConf &conf)
{
    MZX_CHECK(master != nullptr && sock_ == -1);

    return true;
}

void NetAcceptor::Unint()
{
    if (sock_ != -1)
    {
        close(sock_);
        sock_ = -1;
    }
    recv_cb_ = nullptr;
    connect_cb_ = nullptr;
    disconnect_cb_ = nullptr;
}

void NetAcceptor::OnRecv(NetConnectionID connection_id, NetBuffer *buffer)
{
    MZX_CHECK(buffer != nullptr);
    if (recv_cb_)
    {
        recv_cb_(connection_id, buffer->data, buffer->end_pos);
    }
}

void NetAcceptor::OnConnected(NetConnectionID connection_id,
                              const NetAddress &addr)
{
    if (connect_cb_)
    {
        connect_cb_(connection_id, addr);
    }
}

void NetAcceptor::OnDisconnected(NetConnectionID connection_id, NetError error)
{
    if (disconnect_cb_)
    {
        disconnect_cb_(connection_id, error);
    }
}

} // namespace mzx
