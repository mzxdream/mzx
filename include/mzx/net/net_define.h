#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <arpa/inet.h>

namespace mzx
{

using NetMsgLenType = uint16_t;
constexpr std::size_t NET_MSG_MAX_LENGTH = 0xFFF0;
inline NetMsgLenType ToNetMsgLen(NetMsgLenType len)
{
    return htons(len);
}
inline NetMsgLenType ToHostMsgLen(NetMsgLenType len)
{
    return ntohs(len);
}

} // namespace mzx

#endif
