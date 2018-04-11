#ifndef __MZX_EVENTS_H__
#define __MZX_EVENTS_H__

#include <mzx/event.h>

namespace mzx {

template <typename T, typename O>
class Events
{
private:
    Events()
    {
    }
};

template <typename T, typename R, typename ...Args>
class Events<T, R (Args...)>
{
public:
    using EventType = Event<R (Args...)>;
    using Listener = typename EventType::Listener;
public:
    Events()
    {
    }
    ~Events()
    {
    }
    Events(const Events &) = delete;
    Events & operator=(const Events &) = delete;
public:
    EventID AddListener(T type, const Listener &listener)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            auto event = new EventType();
            event_list_[type] = event;
            return event->AddListener(listener);
        }
        return iter_event->second->AddListener(listener);
    }
    void RemoveListener(T type, EventID id)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            return;
        }
        return iter_event->second->RemoveListener(id);
    }
    void RemoveEvent(T type)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            return;
        }
        delete iter_event->second;
        event_list_.erase(iter_event);
    }
    void RemoveAllEvent()
    {
        auto iter_event = event_list_.begin();
        while (iter_event != event_list_.end())
        {
            delete iter_event->second;
            iter_event = event_list_.erase(iter_event);
        }
    }
    void Invoke(T type, Args ...args) const
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            return;
        }
        iter_event->second->Invoke(args...);
    }
public:
    std::map<T, EventType *> event_list_;
};

}

#endif
