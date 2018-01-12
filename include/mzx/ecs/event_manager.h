#ifndef __MZX_ECS_EVENT_MANAGER_H__
#define __MZX_ECS_EVENT_MANAGER_H__

#include <mzx/ecs/event.h>

namespace mzx {
namespace ecs {

class EventManager
{
public:
    EventManager()
    {
    }
    ~EventManager()
    {
        RemoveAllEvent();
    }
    EventManager(const EventManager &) = delete;
    EventManager & operator=(const EventManager &) = delete;
public:
    template <typename T, typename EventType = Event<const T &> >
    Event::IDType AddListener(const EventType::Listener &listener)
    {
        auto iter_event = event_list_.find(EventType::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            auto event = new EventType();
            event_list_[EventType::CLASS_INDEX] = event;
            return event->AddListener(listener);
        }
        auto event = static_cast<EventType *>(iter_event->second);
        return event->AddListener(listener);
    }
    template <typename T, typename EventType = Event<const T &> >
    void RemoveListener(Event::IDType id)
    {
        auto iter_event = event_list_.find(EventType::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            return;
        }
        auto event = static_cast<EventType *>(iter_event->second);
        event->RemoveListener(id);
    }
    template <typename T, typename EventType = Event<const T &> >
    void RemoveEvent()
    {
        auto iter_event = event_list_.find(EventType::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            return;
        }
        delete iter_event->second;
        event_list_.erase(iter_event);
    }
    void RemoveAllEvent()
    {
        for (auto &iter_event : event_list_)
        {
            delete iter_event->second;
        }
        event_list_.clear();
    }
private:
    std::map<EventBase::ClassIndexType, EventBase *> event_list_;
};

}
}

#endif
