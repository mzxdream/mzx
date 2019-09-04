#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

#include <list>
#include <vector>

#include <mzx/data_structure/list.h>
#include <mzx/net/net_define.h>
#include <mzx/net/net_event.h>
#include <mzx/net/net_socket.h>
#include <mzx/spsc_queue.h>
#include <mzx/thread.h>

namespace mzx
{

class NetWorker;

struct NetHandler
{
    int events{0};
    int active_events{0};
    bool (*event_cb)(NetWorker *, NetHandler *){nullptr};
    ListNode list_link;
};

struct NetAcceptor
{
    NetConnectionID id{kNetConnectionIDInvalid};
    NetSocketID sock{kNetSocketIDInvalid};
    NetHandler handler;
};

struct NetPeerConnector
{
    NetConnectionID id{kNetConnectionIDInvalid};
    NetSocketID sock{kNetSocketIDInvalid};
    NetBuffer *read_buffer{nullptr};
    std::list<NetBuffer *> write_buffer_list;
    Error error;
    NetHandler handler;
};

struct NetConnector
{
    NetConnectionID id{kNetConnectionIDInvalid};
    NetSocketID sock{kNetSocketIDInvalid};
    NetAcceptor *acceptor{nullptr};
    NetBuffer *read_buffer{nullptr};
    std::list<NetBuffer *> write_buffer_list;
    Error error;
    NetHandler handler;
    ListNode list_link;
};

class NetWorker final
{
public:
    NetWorker();
    ~NetWorker();
    NetWorker(const NetWorker &) = delete;
    NetWorker &operator=(const NetWorker &) = delete;

public:
    // master thread
    bool Init(const NetConf &conf);
    void Uninit();
    std::size_t InputEventWriteAvailable() const;
    NetBuffer *GetFreeInputBuffer();
    void FreeOutputBuffer(NetBuffer *buffer);
    std::size_t
    HandleOutputEvent(std::function<void(const NetOutputEvent &)> cb,
                      std::size_t count = -1);
    bool AddInputEvent(const NetInputEvent &event);
    bool GetOutputEvent(NetOutputEvent *event);

private:
    // worker thread
    NetBuffer *GetFreeOutputBuffer();
    bool AddOutputEvent(const NetInputEvent &event);
    NetConnector *GetFreeNetConnector();
    void FreeNetConnector(NetConnector *connector);

private:
    NetConnector *GetConnector(NetConnectionID id);
    NetPeerConnector *GetPeerConnector(NetConnectionID id);
    void SendTo(NetConnectionID id, NetBuffer *buffer);
    void Disconnect(NetConnectionID id);
    void FreeInputBuffer(NetBuffer *buffer);
    bool InitIOServer();
    void HandleInputEvent();
    void Wakeup();
    void OnWakeup();
    void Run();

private:
    int epoll_fd_{-1};
    int wakeup_fd_{-1};

    std::vector<NetConnector *> connector_list_;
    List connector_free_list_;
    std::vector<NetAcceptor *> acceptor_list_;
    std::vector<NetPeerConnector *> peer_connector_list_;

    List active_handler_list_;
    SPSCQueue<NetInputEvent> *input_event_list_;
    SPSCQueue<NetOutputEvent> *output_event_list_;
    std::vector<NetBuffer *> input_buffer_list_;
    SPSCQueue<NetBuffer *> *input_buffer_free_list_;
    std::vector<NetBuffer *> output_buffer_list_;
    SPSCQueue<NetBuffer *> *output_buffer_free_list_;
    std::atomic<bool> stop_flag_{false};
    Thread thread_;
};

} // namespace mzx

#endif
