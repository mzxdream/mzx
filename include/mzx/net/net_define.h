#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <cstdint>
#include <string>

namespace mzx
{

using NetID = uint64_t;

enum class NetError
{
    kSuccess,
    kUnknown,
};

enum class NetInputType : char
{
    kSendMessage,
    kStartListen,
    kStopListen,
    kConnect,
    kDisconnect,
};

struct NetInput
{
    NetInputType type;
    int fd;
    char *data;
    std::size_t size;
};

enum class NetOutputType : char
{
    kRecvMessage,
    kConnect,
    kNewConnect,
    kDisconnect,
};

struct NetOutput
{
    NetOutputType type;
    NetError error;
};

struct NetAcceptorConfInfo
{
    std::string ip;
    short port;
};

struct NetPeerConnectorConfInfo
{
    std::string remote_ip;
    short remote_port;
};

class NetConnector
{
};

class NetPeerConnector
{
};

class NetAcceptor
{
};

} // namespace mzx

#endif
