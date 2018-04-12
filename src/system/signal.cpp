#include <mzx/system/signal.h>
#include <map>

namespace mzx {

static std::map<SignalType, Signal::Callback> signal_callback_list;

static void OnSignal(SignalType type)
{
    auto iter_callback = signal_callback_list.find(type);
    if (iter_callback != signal_callback_list.end())
    {
        if (iter_callback->second)
        {
            (iter_callback->second)(type);
        }
    }
}

void Signal::Hook(SignalType type, const Signal::Callback &callback)
{
    signal_callback_list[type] = callback;
    signal(type, OnSignal);
}

void Signal::Unhook(SignalType type)
{
    signal_callback_list.erase(type);
    signal(type, 0);
}

void Signal::UnhookAll()
{
    for (auto &iter_callback : signal_callback_list)
    {
        signal(iter_callback.first, 0);
    }
    signal_callback_list.clear();
}

}
