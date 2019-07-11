#ifndef __MZX_ERROR_H__
#define __MZX_ERROR_H__

#include <mzx/singleton.h>
#include <system_error>

namespace mzx
{

enum class ErrorCode
{
    kSuccess = 0,
    kUnknown,
    kReadOverLimit,
    kWriteOverLimit,
};

class ErrorCategory : public std::error_category
{
public:
    virtual const char *name() const noexcept override;
    virtual std::string message(int err) const override;
};

inline const std::error_category &GetErrorCategory()
{
    return mzx::Singleton<ErrorCategory>::Instance();
}

} // namespace mzx

namespace std
{

template <>
struct is_error_code_enum<mzx::ErrorCode> : true_type
{
};

error_code make_error_code(mzx::ErrorCode ec)
{
    return {
        static_cast<int>(ec),
        mzx::GetErrorCategory(),
    };
}

} // namespace std

#endif
