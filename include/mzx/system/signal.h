#ifndef __MZX_SIGNAL_H__
#define __MZX_SIGNAL_H__

#include <functional>
#include <ostream>
#include <signal.h>

namespace mzx
{

enum class SignalType : int
{
    Interrupt = SIGINT,
    Terminal = SIGTERM,
};

std::ostream &operator<<(std::ostream &os, const SignalType &obj)
{
    os << static_cast<std::underlying_type<SignalType>::type>(obj);
    return os;
}

class Signal
{
public:
    using Callback = std::function<void(SignalType)>;

public:
    static void Hook(SignalType type, Callback cb);
    static void Unhook(SignalType type);
    static void UnhookAll();
};

} // namespace mzx

#endif
