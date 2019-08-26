#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

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
    NetHandler handler;
};

struct NetConnector
{
    NetConnectionID id{kNetConnectionIDInvalid};
    NetSocketID sock{kNetSocketIDInvalid};
    NetAcceptor *acceptor{nullptr};
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
    bool InputEventWriteAvailable() const;
    NetBuffer *GetFreeInputBuffer(std::size_t size);
    void FreeOutputBuffer(NetBuffer *buffer);
    std::size_t
    HandleOutputEvent(std::function<void(const NetOutputEvent &)> cb,
                      std::size_t count = -1);
    bool AddInputEvent(const NetInputEvent &event);

private:
    // worker thread
    NetBuffer *GetFreeOutputBuffer(std::size_t size);
    void FreeInputBuffer(NetBuffer *buffer);
    std::size_t HandleInputEvent(std::function<void(const NetOutputEvent &)> cb,
                                 std::size_t count = -1);
    bool AddOutputEvent(const NetInputEvent &event);
    NetConnector *GetFreeNetConnector(bool force = false);
    void FreeNetConnector(NetConnector *connector);

private:
    bool InitIOServer();
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
