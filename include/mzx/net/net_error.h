#ifndef __MZX_NET_ERROR_H__
#define __MZX_NET_ERROR_H__

namespace mzx
{

enum class NetError
{
    kSuccess,
    kUnknown,
};

inline NetError ToNetError(int error)
{
    switch (error)
    {
        case 0:
            return NetError::kSuccess;
        default:
            return NetError::kUnknown;
    }
}

} // namespace mzx

#endif
