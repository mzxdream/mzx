#ifndef __MZX_NET_EVENT_H__
#define __MZX_NET_EVENT_H__

#include <mzx/error.h>
#include <mzx/net/net_address.h>
#include <mzx/net/net_buffer.h>
#include <mzx/net/net_define.h>
#include <mzx/net/net_socket.h>

namespace mzx
{

enum class NetInputEventType : char
{
    kSend,
    kAccept,
    kPeerConnect,
    kDisconnect,
};

struct NetEventSend final
{
    NetBuffer *buffer;
};

struct NetEventAccept
{
    NetSocketID sock;
};

struct NetEventPeerConnect
{
    NetAddress addr;
    int64_t outdate;
    int64_t retry_interval;
    bool auto_reconnect;
};

struct NetInputEvent
{
    NetInputEventType type{NetInputEventType::kSend};
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
    Error error;
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
