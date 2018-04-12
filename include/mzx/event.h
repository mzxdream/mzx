#ifndef __MZX_EVENT_H__
#define __MZX_EVENT_H__

#include <cstddef>
#include <functional>
#include <map>

namespace mzx {

using EventID = std::size_t;
const static EventID EVENT_ID_INVALID = (EventID)-1;

template <typename T>
class Event
{
private:
    Event()
    {
    }
    ~Event()
    {
    }
    Event(const Event &) = delete;
    Event & operator=(const Event &) = delete;
};

template <typename R, typename ...Args>
class Event<R (Args...)>
{
public:
    using Listener = std::function<R (Args...)>;
public:
    Event()
        : next_id_(0)
    {
    }
    ~Event()
    {
    }
    Event(const Event &) = delete;
    Event & operator=(const Event &) = delete;
public:
    EventID AddListener(const Listener &listener)
    {
        EventID id = ++next_id_;
        listener_list_[id] = listener;
        return id;
    }
    void RemoveListener(EventID id)
    {
        listener_list_.erase(id);
    }
    void RemoveAllListener()
    {
        listener_list_.clear();
    }
    void Invoke(Args ...args) const
    {
        auto iter_listener = listener_list_.begin();
        while (iter_listener != listener_list_.end())
        {
            auto &listener = iter_listener->second;
            ++iter_listener;
            if (listener)
            {
                listener(args...);
            }
        }
    }
private:
    std::map<EventID, Listener> listener_list_;
    EventID next_id_;
};

template <typename T, typename O>
class EventManager
{
private:
    EventManager()
    {
    }
};

template <typename T, typename R, typename ...Args>
class EventManager<T, R (Args...)>
{
public:
    using EventType = Event<R (Args...)>;
    using Listener = typename EventType::Listener;
public:
    EventManager()
    {
    }
    ~EventManager()
    {
    }
    EventManager(const EventManager &) = delete;
    EventManager & operator=(const EventManager &) = delete;
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
