#ifndef __MZX_ECS_EVENT_MANAGER_H__
#define __MZX_ECS_EVENT_MANAGER_H__

#include <mzx/ecs/event.h>

namespace mzx {
namespace ecs {

class EventManager
{
public:
    template <typename T, typename ...Args>
    Event::IDType AddListener(const std::function<void (Args...)> &listener)
    {
        auto iter_event = event_list_.find(Event<T>::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            auto event = new Event<T, Args...>();
            event_list_[Event<T>::CLASS_INDEX] = event;
            return event->AddListener(listener);
        }
        auto event = static_cast<Event<T, Args...> *>(iter_event->second);
        return event->AddListener(listener);
    }
    template <typename T>
    void RemoveListener(Event::IDType id)
    {
        auto iter_event = event_list_.find(Event<T>::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            return;
        }
        iter_event->second
    }
private:
    std::map<EventBase::ClassIndexType, EventBase *> event_list_;
};

}
}

#endif
