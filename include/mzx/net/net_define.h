#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <arpa/inet.h>

namespace mzx
{

using NetMessageLengthType = uint16_t;
constexpr std::size_t kNetMessageLengthSize = sizeof(NetMessageLengthType);
constexpr std::size_t kNetMessageMaxLength = 0xFFF0;

inline NetMessageLengthType ToNetMessageLength(NetMessageLengthType len)
{
    return htons(len);
}

inline NetMessageLengthType ToHostMessageLength(NetMessageLengthType len)
{
    return ntohs(len);
}

} // namespace mzx

#endif
