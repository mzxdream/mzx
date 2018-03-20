#ifndef __MZX_SIGNAL_H__
#define __MZX_SIGNAL_H__

#include <functional>

namespace mzx {

class Signal
{
public:
    using Type = int;
    using Callback = std::function<void (Type)>;
public:
    static void Hook(Type type, const Callback &callback);
    static void Unhook(Type type);
    static void UnhookAll();
};

}

#endif
