#include <mzx/net/net_master.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetMaster::NetMaster()
{
}

NetMaster::~NetMaster()
{
    Uninit();
}

bool NetMaster::Init(const NetConf &conf)
{
    MZX_CHECK(conf.worker_count < kNetWorkerMaxCount);
    for (std::size_t i = 0; i < conf.worker_count; ++i)
    {
        auto *worker = new NetWorker(i);
        if (!worker->Init(conf))
        {
            delete worker;
            return false;
        }
        worker_list_.emplace_back(worker);
    }
    return true;
}

void NetMaster::Uninit()
{
    for (auto *worker : worker_list_)
    {
        worker->Uninit();
        delete worker;
    }
    worker_list_.clear();
    for (auto *acceptor : acceptor_list_)
    {
        NetSocket::DestroySocket(acceptor->sock);
        delete acceptor;
    }
    acceptor_list_.clear();
    for (auto *peer_connector : peer_connector_list_)
    {
        delete peer_connector;
    }
    peer_connector_list_.clear();
}

void NetMaster::HandleRecvEvent(NetWorker *worker, NetConnectionID id,
                                NetConnectionID handler_id,
                                const NetEventRecv &event)
{
    MZX_CHECK(worker != nullptr && event.buffer != nullptr);
    auto handler_type = ParseNetConnectionType(handler_id);
    auto handler_index = ParseNetConnectionIndex(handler_id);
    if (handler_type == NetConnectionType::kAcceptor)
    {
        if (handler_index < acceptor_list_.size())
        {
            auto *acceptor = acceptor_list_[handler_index];
            if (acceptor && acceptor->id == handler_id &&
                acceptor->conf.recv_cb)
            {
                (acceptor->conf.recv_cb)(id, event.buffer->ReadBegin(),
                                         event.buffer->ReadAvailable());
            }
        }
    }
    else if (handler_type == NetConnectionType::kPeerConnector)
    {
        if (handler_index < peer_connector_list_.size())
        {
            auto *peer_connector = peer_connector_list_[handler_index];
            if (peer_connector && peer_connector->id == handler_id &&
                peer_connector->conf.recv_cb)
            {
                (peer_connector->conf.recv_cb)(id, event.buffer->ReadBegin(),
                                               event.buffer->ReadAvailable());
            }
        }
    }
    else
    {
        MZX_ERR("unknown handler type:", handler_type);
    }
    worker->FreeOutputBuffer(event.buffer);
}

void NetMaster::HandleConnectedEvent(NetWorker *worker, NetConnectionID id,
                                     NetConnectionID handler_id,
                                     const NetEventConnected &event)
{
    MZX_CHECK(worker != nullptr);
    auto handler_type = ParseNetConnectionType(handler_id);
    auto handler_index = ParseNetConnectionIndex(handler_id);
    if (handler_type == NetConnectionType::kAcceptor)
    {
        if (handler_index < acceptor_list_.size())
        {
            auto *acceptor = acceptor_list_[handler_index];
            if (acceptor && acceptor->id == handler_id &&
                acceptor->conf.connect_cb)
            {
                (acceptor->conf.connect_cb)(id, event.addr.IP(),
                                            event.addr.Port());
            }
        }
    }
    else if (handler_type == NetConnectionType::kPeerConnector)
    {
        if (handler_index < peer_connector_list_.size())
        {
            auto *peer_connector = peer_connector_list_[handler_index];
            if (peer_connector && peer_connector->id == handler_id &&
                peer_connector->conf.connect_cb)
            {
                (peer_connector->conf.connect_cb)(id, event.addr.IP(),
                                                  event.addr.Port());
            }
        }
    }
    else
    {
        MZX_ERR("unknown handler type:", handler_type);
    }
}

void NetMaster::HandleDisconnectedEvent(NetWorker *worker, NetConnectionID id,
                                        NetConnectionID handler_id,
                                        const NetEventDisconnected &event)
{
    MZX_CHECK(worker != nullptr);
    auto handler_type = ParseNetConnectionType(handler_id);
    auto handler_index = ParseNetConnectionIndex(handler_id);
    if (handler_type == NetConnectionType::kAcceptor)
    {
        if (handler_index < acceptor_list_.size())
        {
            auto *acceptor = acceptor_list_[handler_index];
            if (acceptor && acceptor->id == handler_id &&
                acceptor->conf.disconnect_cb)
            {
                (acceptor->conf.disconnect_cb)(id, event.error);
            }
        }
    }
    else if (handler_type == NetConnectionType::kPeerConnector)
    {
        if (handler_index < peer_connector_list_.size())
        {
            auto *peer_connector = peer_connector_list_[handler_index];
            if (peer_connector && peer_connector->id == handler_id &&
                peer_connector->conf.disconnect_cb)
            {
                (peer_connector->conf.disconnect_cb)(id, event.error);
            }
        }
    }
    else
    {
        MZX_ERR("unknown handler type:", handler_type);
    }
}

void NetMaster::Update()
{
    NetOutputEvent event;
    for (auto *worker : worker_list_)
    {
        MZX_CHECK(worker != nullptr);
        for (std::size_t j = 0; j < conf_.handle_count; ++j)
        {
            if (!worker->GetOutputEvent(&event))
            {
                break;
            }
            switch (event.type)
            {
                case NetOutputEventType::kRecv:
                {
                    HandleRecvEvent(worker, event.id, event.handler_id,
                                    event.data.recv_event);
                }
                break;
                case NetOutputEventType::kConnected:
                {
                    HandleConnectedEvent(worker, event.id, event.handler_id,
                                         event.data.connected_event);
                }
                break;
                case NetOutputEventType::kDisConnected:
                {
                    HandleDisconnectedEvent(worker, event.id, event.handler_id,
                                            event.data.disconnected_event);
                }
                break;
                default:
                {
                    MZX_ERR("unknown output event type:", event.type);
                }
                break;
            }
        }
    }
}

NetConnectionID NetMaster::AddAcceptor(const NetAcceptorConf &conf)
{
    MZX_CHECK(acceptor_list_.size() < kNetConnectionMaxCount &&
              worker_list_.size() > 0);
    NetAddress bind_addr(conf.bind_port, conf.bind_ip.c_str(), conf.is_ipv6);
    Error error = Error::kSuccess;
    auto sock = NetSocket::CreateTcpSocket(bind_addr,
                                           kNetSocketFlagNonBlock |
                                               kNetSocketFlagCloseOnExec |
                                               kNetSocketFlagReuseAddr,
                                           &error);
    if (sock == kNetSocketIDInvalid)
    {
        MZX_ERR("create sock failed error:", error);
        return kNetSocketIDInvalid;
    }
    error = NetSocket::Listen(sock);
    if (error != Error::kSuccess)
    {
        MZX_ERR("sock:", sock, " listen failed error:", error);
        NetSocket::DestroySocket(sock);
        return kNetConnectionIDInvalid;
    }
    auto *acceptor = new NetAcceptorInfo;
    acceptor->id = InitNetConnectionID(NetConnectionType::kAcceptor, 0,
                                       acceptor_list_.size());
    acceptor->sock = sock;
    acceptor->conf = conf;
    acceptor_list_.emplace_back(acceptor);
    NetInputEvent event;
    event.type = NetInputEventType::kAccept;
    event.id = acceptor->id;
    event.data.accept_event.sock = sock;
    for (std::size_t i = 0; i < worker_list_.size(); ++i)
    {
        if (!worker_list_[i]->AddInputEvent(event))
        {
            MZX_ERR("worker index:", i, " add input event failed");
            continue;
        }
    }
    return acceptor->id;
}

NetConnectionID NetMaster::AddPeerConnector(const NetPeerConnectorConf &conf)
{
    MZX_CHECK(peer_connector_list_.size() < kNetConnectionMaxCount &&
              worker_list_.size() > 0);
    auto worker_index = peer_connector_list_.size() % worker_list_.size();
    auto *worker = worker_list_[worker_index];
    MZX_CHECK(worker != nullptr);
    if (worker->InputEventWriteAvailable() == 0)
    {
        MZX_ERR("worker index:", worker_index, " input event is full");
        return kNetConnectionIDInvalid;
    }
    auto *peer_connector = new NetPeerConnectorInfo;
    peer_connector->id = InitNetConnectionID(NetConnectionType::kPeerConnector,
                                             0, peer_connector_list_.size());
    peer_connector->worker_index = worker_index;
    peer_connector->conf = conf;
    peer_connector_list_.emplace_back(peer_connector);
    NetInputEvent event;
    event.type = NetInputEventType::kPeerConnect;
    event.id = peer_connector->id;
    event.data.peer_connect_event.addr =
        NetAddress(conf.remote_port, conf.remote_ip.c_str(), conf.is_ipv6);
    if (!worker->AddInputEvent(event))
    {
        MZX_ERR("worker index:", worker_index, " add input event failed");
        return kNetConnectionIDInvalid;
    }
    return peer_connector->id;
}

bool NetMaster::Send(NetConnectionID id, const char *data, std::size_t size)
{
    MZX_CHECK(data != nullptr && size != 0 && size <= conf_.input_buffer_size);
    auto connection_type = ParseNetConnectionType(id);
    std::size_t worker_index = 0;
    if (connection_type == NetConnectionType::kConnector)
    {
        worker_index = ParseNetWorkerIndex(id);
    }
    else if (connection_type == NetConnectionType::kPeerConnector)
    {
        auto index = ParseNetConnectionIndex(id);
        if (index >= peer_connector_list_.size())
        {
            MZX_ERR("peer connector index is overflow id:", id,
                    " index:", index, " size:", peer_connector_list_.size());
            return false;
        }
        worker_index = peer_connector_list_[index]->worker_index;
    }
    else
    {
        MZX_FATAL("connection_type is unknown type:", connection_type);
    }
    MZX_CHECK(worker_index < worker_list_.size());
    auto *worker = worker_list_[worker_index];
    MZX_CHECK(worker != nullptr);
    if (worker->InputEventWriteAvailable() == 0)
    {
        MZX_ERR("worker:", worker_index, " input event is full");
        return false;
    }
    auto *buffer = worker->GetFreeInputBuffer();
    if (!buffer)
    {
        MZX_ERR("buffer is full worker:", worker_index);
        return false;
    }
    MZX_CHECK(buffer->WriteAvailable() >= size);
    memcpy(buffer->WriteBegin(), data, size);
    buffer->Write(size);
    NetInputEvent event;
    event.type = NetInputEventType::kSend;
    event.id = id;
    event.data.send_event.buffer = buffer;
    if (!worker->AddInputEvent(event))
    {
        MZX_ERR("worker index:", worker_index, " add input event failed");
        return false;
    }
    return true;
}

bool NetMaster::Disconnect(NetConnectionID id)
{
    auto connection_type = ParseNetConnectionType(id);
    MZX_CHECK(connection_type == NetConnectionType::kConnector);
    auto worker_index = ParseNetWorkerIndex(id);
    MZX_CHECK(worker_index < worker_list_.size());
    auto *worker = worker_list_[worker_index];
    MZX_CHECK(worker != nullptr);
    if (worker->InputEventWriteAvailable() == 0)
    {
        MZX_ERR("worker:", worker_index, " input event is full");
        return false;
    }
    NetInputEvent event;
    event.type = NetInputEventType::kDisconnect;
    event.id = id;
    if (!worker->AddInputEvent(event))
    {
        MZX_ERR("worker index:", worker_index, " add input event failed");
        return false;
    }
    return true;
}

} // namespace mzx
