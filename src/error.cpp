#include <errno.h>

#include <mzx/error.h>

namespace mzx
{

static ErrorType ToErrorType(int err)
{
    switch (err)
    {
        default:
            return ErrorType::Unknown;
    }
}

Error::Error(int eno)
    : type_(ToErrorType(eno))
{
}

Error::Error(ErrorType t)
    : type_(t)
{
}

Error::Error(ErrorType t, const std::string &msg)
    : type_(t)
    , message_(msg)
{
}

ErrorType Error::Type() const
{
    return type_;
}

const std::string &Error::Message() const
{
    return message_;
}

void Error::SetType(int eno)
{
    type_ = ToErrorType(eno);
}

void Error::SetType(ErrorType t)
{
    type_ = t;
}

void Error::SetMessage(const std::string &msg)
{
    message_ = msg;
}

} // namespace mzx
