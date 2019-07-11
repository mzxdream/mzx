#include <mzx/error.h>

namespace mzx
{

const char *ErrorCategory::name() const noexcept
{
    return "mzx::ErrorCategory";
}

std::string ErrorCategory::message(int err) const
{
    switch (err)
    {
        default:
            return "Unknown";
    }
}

} // namespace mzx
