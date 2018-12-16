#ifndef __MZX_EVENT_H__
#define __MZX_EVENT_H__

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <cassert>
#include <mzx/list.h>

namespace mzx {

using EventID = std::size_t;
constexpr static EventID EVENT_ID_INVALID = (EventID)-1;

template <typename T>
class Event;

template <typename R, typename ...Args>
class Event<R (Args...)>
{
public:
    using Listener = std::function<R (Args...)>;
private:
    struct ListenerNode
    {
        ListenerNode(Listener l)
            : listener(l)
            , ref_count(1)
        {
            assert(listener != nullptr);
            MZX_INIT_LIST_HEAD(&list_link);
        }
        ~ListenerNode()
        {
            assert(listener == nullptr && ref_count == 0);
            MZX_LIST_REMOVE(&list_link);
        }
        void IncrRef()
        {
            ++ref_count;
        }
        void DecrRef()
        {
            assert(ref_count > 0);
            if (--ref_count == 0)
            {
                delete this;
            }
        }
        void SelfRemove()
        {
            if (listener != nullptr)
            {
                listener = nullptr;
                DecrRef();
            }
        }
        Listener listener;
        int ref_count{ 0 };
        ListHead list_link;
    };
public:
    Event()
    {
        MZX_INIT_LIST_HEAD(&listener_list_);
    }
    ~Event()
    {
        RemoveAllListener();
    }
    Event(const Event &) = delete;
    Event & operator=(const Event &) = delete;
public:
    EventID AddListener(const Listener &listener)
    {
        static_assert(sizeof(ListenerNode *) == sizeof(EventID));
        auto *node = new ListenerNode(listener);
        MZX_LIST_PUSH_BACK(&node->list_link, &listener_list_);
        return (EventID)node;
    }
    void RemoveListener(EventID id)
    {
        MZX_LIST_FOREACH(it, &listener_list_)
        {
            auto *node = MZX_LIST_ENTRY(it, ListenerNode, list_link);
            if ((EventID)node == id)
            {
                node->SelfRemove();
                break;
            }
        }
    }
    void RemoveAllListener()
    {
        MZX_LIST_FOREACH_SAFE(it, &listener_list_)
        {
            auto *node = MZX_LIST_ENTRY(it, ListenerNode, list_link);
            node->SelfRemove();
        }
    }
    void Invoke(Args ...args) const
    {
        for (auto it = MZX_LIST_BEGIN(&listener_list_); it != MZX_LIST_END(&listener_list_);) 
        {
            auto *node = MZX_LIST_ENTRY(it, ListenerNode, list_link);
            node->IncrRef();
            if (node->listener)
            {
                (node->listener)(args...);
            }
            it = MZX_LIST_NEXT(it);
            node->DecrRef();
        }
    }
private:
    ListHead listener_list_;
};

template <typename T, typename O>
class Events;

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
        RemoveAllEvent();
    }
    Events(const Events &) = delete;
    Events & operator=(const Events &) = delete;
public:
    EventID AddListener(T type, const Listener &listener)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            auto *event = new EventType();
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
        auto *event = iter_event->second;
        event_list_.erase(iter_event);
        delete event;
    }
    void RemoveAllEvent()
    {
        for (auto iter_event = event_list_.begin(); iter_event != event_list_.end(); iter_event = event_list_.begin())
        {
            auto *event = iter_event->second;
            event_list_.erase(iter_event);
            delete event;
        }
    }
    void Invoke(T type, Args ...args) const
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            return;
        }
        iter_event->second->Invoke(std::forward<Args>(args)...);
    }
public:
    std::unordered_map<T, EventType *> event_list_;
};

}

#endif
