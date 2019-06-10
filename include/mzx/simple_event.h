#ifndef __MZX_SIMPLE_EVENT_H_
#define __MZX_SIMPLE_EVENT_H_

#include <mzx/event.h>

namespace mzx
{

class SimpleEventBase
{
public:
    using ClassIndexType = std::size_t;

public:
    SimpleEventBase();
    virtual ~SimpleEventBase() = 0;
    SimpleEventBase(const SimpleEventBase &) = delete;
    SimpleEventBase &operator=(const SimpleEventBase &) = delete;

public:
    static ClassIndexType ClassIndexCount();
    virtual ClassIndexType ClassIndex() const = 0;

protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class SimpleEvent : public SimpleEventBase
{
public:
    const static ClassIndexType CLASS_INDEX;

public:
    SimpleEvent()
    {
    }
    virtual ~SimpleEvent()
    {
    }
    SimpleEvent(const SimpleEvent &) = delete;
    SimpleEvent &operator=(const SimpleEvent &) = delete;

public:
    virtual ClassIndexType ClassIndex() const override
    {
        return CLASS_INDEX;
    }
};

template <typename T>
const SimpleEventBase::ClassIndexType SimpleEvent<T>::CLASS_INDEX =
    SimpleEventBase::class_index_counter_++;

class SimpleEventManager final
{
public:
    using Listener = std::function<void(const SimpleEventBase *)>;

    SimpleEventManager();
    ~SimpleEventManager();
    SimpleEventManager(const SimpleEventManager &) = delete;
    SimpleEventManager &operator=(const SimpleEventManager &) = delete;

public:
    template <typename T>
    EventID AddListener(Listener listener)
    {
        MZX_CHECK_STATIC(std::is_base_of<SimpleEvent<T>, T>::value);
        return event_manager_.AddListener(T::CLASS_INDEX, listener);
    }
    template <typename T>
    void RemoveListener(EventID id)
    {
        MZX_CHECK_STATIC(std::is_base_of<SimpleEvent<T>, T>::value);
        return event_manager_.RemoveListener(T::CLASS_INDEX, id);
    }
    template <typename T>
    void RemoveEvent()
    {
        MZX_CHECK_STATIC(std::is_base_of<SimpleEvent<T>, T>::value);
        return event_manager_.RemoveEvent(T::CLASS_INDEX);
    }
    void RemoveAllEvent()
    {
        event_manager_.RemoveAllEvent();
    }
    template <typename T>
    void Invoke(const T &arg) const
    {
        event_manager_.Invoke(T::CLASS_INDEX, &arg);
    }
    template <typename T, typename... Args>
    void InvokeEmplace(Args &&... args)
    {
        event_manager_.Invoke(T::CLASS_INDEX, &T(std::forward<Args>(args)...));
    }

private:
    EventManager<SimpleEventBase::ClassIndexType, void(const SimpleEventBase *)>
        event_manager_;
};

} // namespace mzx

#endif
