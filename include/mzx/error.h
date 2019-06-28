#ifndef __MZX_ERROR_H__
#define __MZX_ERROR_H__

#include <string>

namespace mzx
{

enum class ErrorNo
{
    Sucess = 0,
    Unknown = 1,
};

struct Error
{
    Error() = default;
    Error(ErrorNo eno)
        : errorno(eno)
    {
    }
    Error(ErrorNo eno, const std::string &msg)
        : errorno(eno)
        , message(msg)
    {
    }

    operator bool() const
    {
        return errorno != ErrorNo::Sucess;
    }
    bool operator!() const
    {
        return errorno == ErrorNo::Sucess;
    }

    ErrorNo errorno{ErrorNo::Sucess};
    std::string message;
};

}; // namespace mzx

#endif
