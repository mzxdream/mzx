#ifndef __MZX_SIGNAL_H__
#define __MZX_SIGNAL_H__

#include <functional>
#include <signal.h>

namespace mzx
{

using SignalType = int;

class Signal
{
public:
    using Callback = std::function<void(SignalType)>;

public:
    static void Hook(SignalType type, const Callback &callback);
    static void Unhook(SignalType type);
    static void UnhookAll();
};

} // namespace mzx

#endif
