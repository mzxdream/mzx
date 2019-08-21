#ifndef __MZX_ERROR_H__
#define __MZX_ERROR_H__

namespace mzx
{

enum class Error
{
    kSuccess,
    kUnknown,
};

inline Error ToError(int error)
{
    switch (error)
    {
        case 0:
            return Error::kSuccess;
        default:
            return Error::kUnknown;
    }
}

} // namespace mzx

#endif
