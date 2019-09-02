#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetWorker::NetWorker()
    : thread_(std::bind(&NetWorker::Run, this))
{
}

NetWorker::~NetWorker()
{
    Uninit();
}

bool NetWorker::InitIOServer()
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0)
    {
        MZX_ERR("create epoll fd failed");
        return false;
    }
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeup_fd_ < 0)
    {
        MZX_ERR("create wakeup fd failed");
        close(epoll_fd_);
        return false;
    }
    epoll_event ee;
    ee.events = EPOLLIN | EPOLLET;
    ee.data.ptr = this;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, wakeup_fd_, &ee);
    if (ret < 0)
    {
        MZX_ERR("add wakeup fd:", wakeup_fd_, " in efd:", epoll_fd_, " failed");
        close(epoll_fd_);
        close(wakeup_fd_);
        return false;
    }
    return true;
}

bool NetWorker::Init(const NetConf &conf)
{
    MZX_CHECK(conf.connector_count > 0);
    MZX_CHECK(conf.input_event_count > 0);
    MZX_CHECK(conf.output_event_count > 0);
    MZX_CHECK(conf.input_buffer_count > 0 && conf.input_buffer_size > 0);
    MZX_CHECK(conf.output_buffer_count > 0 && conf.output_buffer_size > 0);
    if (!InitIOServer())
    {
        MZX_ERR("init io server failed");
        return false;
    }
    for (std::size_t i = 0; i < conf.connector_count; ++i)
    {
        auto *connector = new NetConnector();
        connector->id = InitNetConnectionID(NetConnectionType::kConnector,
                                            connector_list_.size());
        connector_list_.emplace_back(connector);
        connector_free_list_.PushBack(&connector->list_link);
    }
    input_event_list_ = new SPSCQueue<NetInputEvent>(conf.input_event_count);
    output_event_list_ = new SPSCQueue<NetOutputEvent>(conf.output_event_count);
    input_buffer_free_list_ =
        new SPSCQueue<NetBuffer *>(conf.input_buffer_count);
    for (std::size_t i = 0; i < conf.input_buffer_count; ++i)
    {
        auto *buffer = new NetBuffer(conf.input_buffer_size);
        input_buffer_list_.emplace_back(buffer);
        input_buffer_free_list_->Push(buffer);
    }
    output_buffer_free_list_ =
        new SPSCQueue<NetBuffer *>(conf.output_buffer_count);
    for (std::size_t i = 0; i < conf.output_buffer_count; ++i)
    {
        auto *buffer = new NetBuffer(conf.output_buffer_size);
        output_buffer_list_.emplace_back(buffer);
        output_buffer_free_list_->Push(buffer);
    }
    stop_flag_ = false;
    thread_.Start();
    return true;
}

void NetWorker::Uninit()
{
    stop_flag_.store(true, std::memory_order_relaxed);
    Wakeup();
    thread_.Join();
    if (epoll_fd_ > 0)
    {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    if (wakeup_fd_ > 0)
    {
        close(wakeup_fd_);
        wakeup_fd_ = -1;
    }
    for (std::size_t i = 0; i < connector_list_.size(); ++i)
    {
        // TODO
    }
    // TODO
    delete input_event_list_;
    delete output_event_list_;
    delete input_buffer_free_list_;
    delete output_buffer_free_list_;
}

std::size_t NetWorker::InputEventWriteAvailable() const
{
    return input_event_list_->WriteAvailable();
}

NetBuffer *NetWorker::GetFreeInputBuffer()
{
    NetBuffer *buffer = nullptr;
    input_buffer_free_list_->Pop(&buffer);
    return buffer;
}

void NetWorker::FreeOutputBuffer(NetBuffer *buffer)
{
    output_buffer_free_list_->Push(buffer);
}

std::size_t
NetWorker::HandleOutputEvent(std::function<void(const NetOutputEvent &)> cb,
                             std::size_t count)
{
    MZX_CHECK(cb != nullptr);
    return output_event_list_->Consume(cb, count);
}

bool NetWorker::AddInputEvent(const NetInputEvent &event)
{
    return input_event_list_->Push(event);
}

NetBuffer *NetWorker::GetFreeOutputBuffer()
{
    NetBuffer *buffer = nullptr;
    output_buffer_free_list_->Pop(&buffer);
    return nullptr;
}

NetConnector *NetWorker::GetFreeNetConnector()
{
    auto *node = connector_free_list_.PopFront();
    if (!node)
    {
        return nullptr;
    }
    auto *connector = MZX_CONTAINER_OF(node, NetConnector, list_link);
    MZX_CHECK(connector != nullptr);
    connector->list_link.Erase();
    return connector;
}

void NetWorker::FreeNetConnector(NetConnector *connector)
{
    MZX_CHECK(connector != nullptr);
    connector->id = NextNetConnectionID(connector->id);
    connector_free_list_.PushBack(&connector->list_link);
}

NetConnector *NetWorker::GetConnector(NetConnectionID id)
{
    MZX_CHECK(ParseNetConnectionType(id) == NetConnectionType::kConnector);
    auto index = ParseNetConnectionIndex(id);
    MZX_CHECK(index < connector_list_.size());
    auto *connector = connector_list_[index];
    if (connector->id != id)
    {
        return nullptr;
    }
    return connector;
}

NetPeerConnector *NetWorker::GetPeerConnector(NetConnectionID id)
{
    return nullptr;
}

static bool EpollCtl(int efd, int fd, int pevents, int events, void *ptr)
{
    int op = EPOLL_CTL_MOD;
    if (pevents & (EPOLLIN | EPOLLOUT))
    {
        if (!(events & (EPOLLIN | EPOLLOUT)))
        {
            op = EPOLL_CTL_DEL;
        }
    }
    else
    {
        if (!(events & (EPOLLIN | EPOLLOUT)))
        {
            MZX_WARN("epoll:", efd, " ctl:", fd, " events:", events,
                     " not has in or out event");
            return false;
        }
        op = EPOLL_CTL_ADD;
    }
    epoll_event ee;
    ee.events = events;
    ee.data.ptr = ptr;
    int ret = epoll_ctl(efd, op, fd, &ee);
    if (ret < 0)
    {
        MZX_ERR("epoll:", efd, " ctl:", fd, " events:", events,
                " failed ret:", ret);
        return false;
    }
    return true;
}

void NetWorker::SendTo(NetConnectionID id, NetBuffer *buffer)
{
    auto connection_type = ParseNetConnectionType(id);
    switch (connection_type)
    {
        case NetConnectionType::kConnector:
        {
            auto *connector = GetConnector(id);
            if (!connector)
            {
                FreeInputBuffer(buffer);
            }
            else
            {
                if (connector->write_buffer_list.empty())
                {
                    // Try write
                }
                //
                connector->write_buffer_list.push_back(buffer);
            }
        }
        break;
        case NetConnectionType::kPeerConnector:
        {
        }
        break;
        default:
        {
            MZX_ERR("invalid connection type:", connection_type, " id:", id);
        }
        break;
    }
}

static bool OnConnectorEvents(NetWorker *worker, NetHandler *handler)
{
    return true;
}

static bool OnAcceptorEvents(NetWorker *worker, NetHandler *handler)
{
    return true;
}

static bool OnPeerConnectorEvents(NetWorker *worker, NetHandler *handler)
{
    return true;
}

void NetWorker::HandleInputEvent()
{
    NetInputEvent event;
    auto size = input_event_list_->ReadAvailable();
    while (size-- > 0 && input_event_list_->Pop(&event))
    {
        switch (event.type)
        {
            case NetInputEventType::kSend:
            {
                SendTo(event.id, event.data.send_event.buffer);
            }
            break;
            case NetInputEventType::kAccept:
            {
                auto index = ParseNetConnectionIndex(event.id);
                for (std::size_t i = acceptor_list_.size(); i <= index; ++i)
                {
                    auto *acceptor = new NetAcceptor();
                    acceptor->id = kNetConnectionIDInvalid;
                    acceptor_list_.emplace_back(acceptor);
                }
                auto *acceptor = acceptor_list_[index];
                if (acceptor->id != kNetConnectionIDInvalid)
                {
                    MZX_ERR("acceptor is exist id:", acceptor->id,
                            " event id:", event.id);
                    break;
                }
                acceptor->id = event.id;
                acceptor->sock = event.data.accept_event.sock;
                acceptor->handler.events = EPOLLIN | EPOLLET;
                acceptor->handler.event_cb = OnAcceptorEvents;
                EpollCtl(epoll_fd_, acceptor->sock, 0, acceptor->handler.events,
                         acceptor);
            }
            break;
            case NetInputEventType::kPeerConnect:
            {
                auto index = ParseNetConnectionIndex(event.id);
                for (std::size_t i = peer_connector_list_.size(); i <= index;
                     ++i)
                {
                    auto *peer_connector = new NetPeerConnector();
                    peer_connector->id = kNetConnectionIDInvalid;
                    peer_connector_list_.emplace_back(peer_connector);
                }
                auto *peer_connector = peer_connector_list_[index];
                if (peer_connector->id != kNetConnectionIDInvalid)
                {
                    MZX_ERR("peer connector is exist id:", peer_connector->id,
                            " event id:", event.id);
                    break;
                }
                auto &peer_connector_data = event.data.peer_connect_event;
                auto sock = NetSocket::CreateTcpSocket(
                    peer_connector_data.addr, nullptr,
                    kNetSocketFlagNonBlock | kNetSocketFlagReuseAddr |
                        kNetSocketFlagCloseOnExec);
                if (sock == kNetSocketIDInvalid)
                {
                    MZX_ERR("create sock failed");
                    break;
                }
                auto error = NetSocket::Connect(sock, peer_connector_data.addr);
                if (error != Error::kSuccess)
                {
                    if (error == Error::kAgain)
                    {
                    }
                }
                peer_connector->id = event.id;
                peer_connector->sock = event.data.accept_event.sock;
                peer_connector->handler.events = EPOLLOUT | EPOLLET;
                peer_connector->handler.event_cb = OnAcceptorEvents;
                EpollCtl(epoll_fd_, peer_connector->sock, 0,
                         peer_connector->handler.events, peer_connector);
            }
            break;
            case NetInputEventType::kDisconnect:
            {
                Disconnect(event.id);
            }
            break;
            default:
            {
                MZX_ERR("unknown input event type:", event.type);
            }
            break;
        }
    }
}

void NetWorker::Wakeup()
{
    uint64_t tmp = 1;
    write(wakeup_fd_, &tmp, sizeof(tmp));
}

void NetWorker::OnWakeup()
{
    uint64_t tmp = 0;
    (void)read(wakeup_fd_, &tmp, sizeof(tmp));
}

void NetWorker::Run()
{
    epoll_event events[1024];
    while (!stop_flag_.load(std::memory_order_relaxed))
    {
        int timeout = -1;
        if (!active_handler_list_.Empty())
        {
            timeout = 0;
        }
        // TODO calculate timeout
        int nfds =
            epoll_wait(epoll_fd_, events, MZX_ARRAY_SIZE(events), timeout);
        if (nfds == -1)
        {
            if (errno != EINTR)
            {
                MZX_FATAL("epoll wait failed:", errno);
            }
        }
        for (int i = 0; i < nfds; ++i)
        {
            auto *p = events[i].data.ptr;
            if (p == this)
            {
                OnWakeup();
                continue;
            }
            auto *handler = static_cast<NetHandler *>(p);
            handler->active_events |= events[i].events;
            if (!handler->list_link.IsLinked())
            {
                active_handler_list_.PushBack(&handler->list_link);
            }
        }
        for (auto *node = active_handler_list_.Begin();
             node != active_handler_list_.End();)
        {
            auto *handler = MZX_CONTAINER_OF(node, NetHandler, list_link);
            MZX_CHECK(handler != nullptr && handler->event_cb != nullptr);
            if (handler->event_cb(this, handler))
            {
                node = active_handler_list_.Erase(node);
            }
            else
            {
                node = node->Next();
            }
        }
    }
}

} // namespace mzx
