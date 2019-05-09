#ifndef __MZX_EVENT_H__
#define __MZX_EVENT_H__

#include <cstddef>
#include <functional>
#include <memory>
#include <mzx/list.h>
#include <mzx/logger.h>
#include <unordered_map>

namespace mzx
{

using EventID = std::size_t;
constexpr static EventID EVENT_ID_INVALID = (EventID)-1;

template <typename T>
class Event;

template <typename R, typename... Args>
class Event<R(Args...)>
{
public:
    using Listener = std::function<R(Args...)>;

private:
    struct ListenerNode
    {
        ListenerNode(Listener l)
            : listener(l)
            , ref_count(1)
        {
            MZX_CHECK(listener != nullptr);
        }
        ~ListenerNode()
        {
            MZX_CHECK(listener == nullptr && ref_count == 0);
        }
        void IncrRef()
        {
            ++ref_count;
        }
        void DecrRef()
        {
            MZX_CHECK(ref_count > 0);
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
        int ref_count{0};
        ListNode list_link;
    };

public:
    Event()
    {
    }
    ~Event()
    {
        RemoveAllListener();
    }
    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;

public:
    EventID AddListener(Listener listener)
    {
        MZX_CHECK_STATIC(sizeof(ListenerNode *) == sizeof(EventID));
        MZX_CHECK(listener != nullptr);
        auto *node = new ListenerNode(listener);
        listener_list_.PushBack(&node->list_link);
        return (EventID)node;
    }
    void RemoveListener(EventID id)
    {
        for (auto *node = listener_list_.Next(); node != &listener_list_; node = node->Next())
        {
            auto *entry = MZX_LIST_ENTRY(node, ListenerNode, list_link);
            if ((EventID)entry == id)
            {
                entry->SelfRemove();
                break;
            }
        }
    }
    void RemoveAllListener()
    {
        for (auto *node = listener_list_.Next(); node != &listener_list_;)
        {
            auto *entry = MZX_LIST_ENTRY(node, ListenerNode, list_link);
            node = node->Next();
            entry->SelfRemove();
        }
    }
    void Invoke(Args... args) const
    {
        for (auto *node = listener_list_.Next(); node != &listener_list_;)
        {
            auto *entry = MZX_LIST_ENTRY(node, ListenerNode, list_link);
            entry->IncrRef();
            if (entry->listener)
            {
                (entry->listener)(args...);
            }
            node = node->Next();
            entry->DecrRef();
        }
    }

private:
    ListNode listener_list_;
};

template <typename T, typename O>
class EventManager;

template <typename T, typename R, typename... Args>
class EventManager<T, R(Args...)>
{
    using EventType = Event<R(Args...)>;
    using Listener = typename EventType::Listener;

public:
    EventManager()
    {
    }
    ~EventManager()
    {
        RemoveAllEvent();
    }
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

public:
    EventID AddListener(T type, Listener listener)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            std::shared_ptr<EventType> event(new EventType());
            event_list_[type] = event;
            return event->AddListener(listener);
        }
        return iter_event->second->AddListener(listener);
    }
    void RemoveListener(T type, EventID id)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event != event_list_.end())
        {
            return iter_event->second->RemoveListener(id);
        }
    }
    void RemoveEvent(T type)
    {
        auto iter_event = event_list_.find(type);
        if (iter_event != event_list_.end())
        {
            iter_event->second->RemoveAllListener();
            event_list_.erase(iter_event);
        }
    }
    void RemoveAllEvent()
    {
        for (auto iter_event = event_list_.begin(); iter_event != event_list_.end();)
        {
            iter_event->second->RemoveAllListener();
            iter_event = event_list_.erase(iter_event);
        }
    }
    void Invoke(T type, Args... args) const
    {
        auto iter_event = event_list_.find(type);
        if (iter_event == event_list_.end())
        {
            return;
        }
        auto event = iter_event->second;
        event->Invoke(std::forward<Args>(args)...);
    }

public:
    std::unordered_map<T, std::shared_ptr<EventType>> event_list_;
};

} // namespace mzx

#endif
