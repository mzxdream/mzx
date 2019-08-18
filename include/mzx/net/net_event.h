#ifndef __MZX_NET_EVENT_H__
#define __MZX_NET_EVENT_H__

#include <mzx/net/net_define.h>

namespace mzx
{

enum class NetInputEventType : char
{
    kSend,
    kAccept,
    kPeerConnect,
    kDisconnect,
};

struct NetEventSend
{
    NetBuffer *buffer{nullptr};
};

struct NetEventAccept
{
    int accept_sock{-1};
};

struct NetEventPeerConnect
{
    NetAddress addr;
    int64_t outdate{-1};
    int64_t retry_interval{-1};
    bool auto_reconnect{false};
};

struct NetInputEvent
{
    NetInputEventType type;
    NetConnectionID id{kNetConnectionIDInvalid};
    union {
        NetEventSend send_event;
        NetEventAccept accept_event;
        NetEventPeerConnect peer_connect_event;
    } data;
};

enum class NetOutputEventType : char
{
    kRecv,
    kConnected,
    kDisConnected,
};

struct NetEventRecv
{
    NetBuffer *buffer{nullptr};
};

struct NetEventConnected
{
    NetAddress addr;
};

struct NetEventDisconnected
{
    NetError error;
};

struct NetOutputEvent
{
    NetOutputEventType type;
    NetConnectionID id{kNetConnectionIDInvalid};
    NetConnectionID handler_id{kNetConnectionIDInvalid};
    union {
        NetEventRecv recv_event;
        NetEventConnected connected_event;
        NetEventDisconnected disconnected_event;
    } data;
};

} // namespace mzx

#endif
