#ifndef __MZX_ERROR_H__
#define __MZX_ERROR_H__

#include <string>

namespace mzx
{

enum class ErrorType
{
    Success = 0,
    Unknown,
    InProgress,
    Interrupt,
    Again,
    ConnectAbort,
    ConnectRefuse,
    RepeatConnect,
    NotConnected,
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

    void SetType(int eno);
    void SetType(ErrorType t);
    void SetMessage(const std::string &msg);

    operator bool() const
    {
        return type_ != ErrorType::Success;
    }
    bool operator!() const
    {
        return type_ == ErrorType::Success;
    }

private:
    ErrorType type_{ErrorType::Success};
    std::string message_;
};

}; // namespace mzx

#endif
