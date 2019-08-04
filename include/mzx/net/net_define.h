#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <cstdint>
#include <string>

namespace mzx
{

using NetID = uint64_t;

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
