#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <cstdint>

#include <mzx/logger.h>

namespace mzx
{

/*
 * type 0xF | worker 0xFFF | incr 0xFFFF | index 0xFFFFFFFF
 */
using NetConnectionID = uint64_t;
constexpr NetConnectionID kNetConnectionIDInvalid = (NetConnectionID)-1;
constexpr std::size_t kNetWorkerMaxCount = 0xFFF;
constexpr std::size_t kNetConnectionMaxCount = 0xFFFFFFFF;

enum class NetConnectionType : char
{
    kConnector = 0,
    kPeerConnector = 1,
    kAcceptor = 2,
};

inline NetConnectionType ParseNetConnectionType(NetConnectionID id)
{
    return (NetConnectionType)(id >> 56);
}

inline std::size_t ParseNetConnectionIndex(NetConnectionID id)
{
    return (std::size_t)(id & 0xFFFFFFFF);
}

inline std::size_t ParseNetWorkerIndex(NetConnectionID id)
{
    return (std::size_t)((id >> 48) & 0xFFF);
}

inline NetConnectionID InitNetConnectionID(NetConnectionType type,
                                           std::size_t worker_index,
                                           std::size_t index)
{
    MZX_CHECK(worker_index <= kNetWorkerMaxCount &&
              index <= kNetConnectionMaxCount);
    return (((NetConnectionID)type) << 56) + (worker_index << 48) + index;
}

inline NetConnectionID NextNetConnectionID(NetConnectionID id)
{
    auto incr = (((id >> 32) + 1) & 0xFFFFFFF) << 32;
    return (id & 0xF0000000FFFFFFFF) + incr;
}

struct NetConf
{
    std::size_t worker_count{1};
    std::size_t connector_count{1024};
    std::size_t input_event_count{2048};
    std::size_t output_event_count{2048};
    std::size_t input_buffer_count{2048};
    std::size_t input_buffer_size{20480};
    std::size_t output_buffer_count{2048};
    std::size_t output_buffer_size{2048};
};

} // namespace mzx

#endif
