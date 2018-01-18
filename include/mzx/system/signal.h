#ifndef __MZX_SYSTEM_SIGNAL_H__
#define __MZX_SYSTEM_SIGNAL_H__

#include <functional>
#include <map>

namespace mzx {
namespace system {

class Signal
{
public:
    using Type = int;
    using Callback = std::function<void (Type)>;
public:
    static void Hook(Type type, const Callback &callback);
    static void Unhook(Type type);
private:
    static void OnSignal(Type type);
private:
    static std::map<Type, Callback> callback_list_;
};

}
}

#endif
