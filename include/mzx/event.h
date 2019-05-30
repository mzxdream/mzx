#ifndef __MZX_EVENT_H__
#define __MZX_EVENT_H__

#include <cstddef>
#include <functional>
#include <memory>

#include <mzx/data_structure/list.h>
#include <mzx/data_structure/list_safe_node.h>
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
    using ListenerNode = ListSafeNode<Listener>;

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
        auto *listener_node = new ListenerNode(new Listener(listener));
        listener_list_.PushBack(&listener_node->list_link_);
        return (EventID)listener_node;
    }
    void RemoveListener(EventID id)
    {
        for (auto *node = listener_list_.Begin(); node != listener_list_.End();
             node = node->Next())
        {
            auto *listener_node =
                MZX_CONTAINER_OF(node, ListenerNode, list_link_);
            if ((EventID)listener_node == id)
            {
                delete listener_node->Detach();
                break;
            }
        }
    }
    void RemoveAllListener()
    {
        for (auto *node = listener_list_.Begin(); node != listener_list_.End();)
        {
            auto *listener_node =
                MZX_CONTAINER_OF(node, ListenerNode, list_link_);
            node = node->Next();
            delete listener_node->Detach();
        }
    }
    void Invoke(Args... args) const
    {
        for (auto *node = listener_list_.Begin(); node != listener_list_.End();)
        {
            auto *listener_node =
                MZX_CONTAINER_OF(node, ListenerNode, list_link_);
            listener_node->IncrRef();
            if (listener_node->Get())
            {
                (*listener_node->Get())(args...);
            }
            node = node->Next();
            listener_node->DecrRef();
        }
    }

private:
    List listener_list_;
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
            event_list_.emplace(type, event);
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
        for (auto iter_event = event_list_.begin();
             iter_event != event_list_.end();)
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
