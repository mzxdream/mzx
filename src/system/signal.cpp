#include <mzx/system/signal.h>
#include <signal.h>

namespace mzx {
namespace system {

std::map<Signal::Type, Signal::Callback> Signal::callback_list_;

void Signal::Hook(Signal::Type type, const Signal::Callback &callback)
{
    callback_list_[type] = callback;
    signal(type, Signal::OnSignal);
}

void Signal::Unhook(Signal::Type type)
{
    callback_list_.erase(type);
    signal(type, 0);
}

void Signal::OnSignal(Signal::Type type)
{
    auto iter_callback = callback_list_.find(type);
    if (iter_callback == callback_list_.end())
    {
        return;
    }
    (iter_callback->second)(type);
}

}
}
