#ifndef __MZX_ERROR_H__
#define __MZX_ERROR_H__

#include <string>

namespace mzx
{

enum class ErrorType
{
    Sucess = 0,
    Unknown = 1,
};

class Error
{
public:
    Error() = default;
    Error(int eno);
    Error(ErrorType t);
    Error(ErrorType t, const std::string &msg);

public:
    ErrorType Type() const;
    const std::string &Message() const;
    operator bool() const
    {
        return type_ != ErrorType::Sucess;
    }
    bool operator!() const
    {
        return type_ == ErrorType::Sucess;
    }

private:
    ErrorType type_{ErrorType::Sucess};
    std::string message_;
};

}; // namespace mzx

#endif
