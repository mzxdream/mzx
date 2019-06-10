#ifndef __MZX_SIGNAL_H__
#define __MZX_SIGNAL_H__

#include <functional>
#include <signal.h>

namespace mzx
{

enum class SignalType : int
{
    Interrupt = SIGINT,
    Terminal = SIGTERM,
};

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
