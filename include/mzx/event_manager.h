#ifndef __MZX_EVENT_MANAGER_H__
#define __MZX_EVENT_MANAGER_H__

#include <mzx/event.h>

namespace mzx {

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
    EventBase::ID AddListener(const typename EventType::Listener &listener)
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
    void RemoveListener(EventBase::ID id)
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
    void Invoke(const T &arg)
    {
        auto iter_event = event_list_.find(EventType::CLASS_INDEX);
        if (iter_event == event_list_.end())
        {
            return;
        }
        auto event = static_cast<EventType *>(iter_event->second);
        event->Invoke(arg);
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
            delete iter_event.second;
        }
        event_list_.clear();
    }
private:
    std::map<EventBase::ClassIndexType, EventBase *> event_list_;
};

}

#endif
