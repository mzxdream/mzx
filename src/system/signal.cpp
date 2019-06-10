#include <map>
#include <mzx/system/signal.h>

namespace mzx
{

static std::map<SignalType, Signal::Callback> signal_callback_list;

static void OnSignal(int type)
{
    auto iter_callback =
        signal_callback_list.find(static_cast<SignalType>(type));
    if (iter_callback != signal_callback_list.end())
    {
        if (iter_callback->second)
        {
            (iter_callback->second)(static_cast<SignalType>(type));
        }
    }
}

void Signal::Hook(SignalType type, Signal::Callback callback)
{
    signal_callback_list[type] = callback;
    signal(static_cast<int>(type), OnSignal);
}

void Signal::Unhook(SignalType type)
{
    signal_callback_list.erase(type);
    signal(static_cast<int>(type), 0);
}

void Signal::UnhookAll()
{
    for (auto &iter_callback : signal_callback_list)
    {
        signal(static_cast<int>(iter_callback.first), 0);
    }
    signal_callback_list.clear();
}

} // namespace mzx
