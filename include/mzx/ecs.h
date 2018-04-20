#ifndef __MZX_ECS_H__
#define __MZX_ECS_H__

#include <list>
#include <map>
#include <cstddef>
#include <utility>
#include <functional>

namespace mzx {

class ComponentBase
{
public:
    using ClassIndexType = std::size_t;
public:
    ComponentBase();
    virtual ~ComponentBase() = 0;
    ComponentBase(const ComponentBase &) = delete;
    ComponentBase & operator=(const ComponentBase &) = delete;
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class Component
    : public ComponentBase
{
public:
    const static ClassIndexType CLASS_INDEX = ++ComponentBase::next_class_index_;
public:
    template <typename ...Args>
    explicit Component(Args && ...args)
        : data_(std::forward<Args>(args)...)
    {
    }
    virtual ~Component()
    {
    }
    Component(const Component &) = delete;
    Component & operator=(const Component &) = delete;
public:
    T * Data() const
    {
        return &data_;
    }
private:
    T data_;
};

template <typename T>
class ComponentHandle
{
public:
    explicit ComponentHandle(T *component = nullptr)
        : component_(component)
    {
    }
public:
    T * operator->() const
    {
        return component_;
    }
    operator bool() const
    {
        return IsValid();
    }
    bool IsValid() const
    {
        return component_ != nullptr;
    }
    T & Data() const
    {
        return *component_;
    }
private:
    T *component_;
};

using EntityID = int64_t;
const static EntityID ENTITY_ID_INVALID = (EntityID)-1;

class Entity
{
public:
    Entity(EntityID id);
    ~Entity();
    Entity(const Entity &) = delete;
    Entity & operator=(const Entity &) = delete;
public:
    EntityID Id() const;
    template <typename T>
    ComponentHandle<T> GetComponent() const
    {
        auto iter_component = component_list_.find(Component<T>::CLASS_INDEX);
        if (iter_component == component_list_.end())
        {
            return ComponentHandle<T>();
        }
        return ComponentHandle<T>(static_cast<Component<T> *>(iter_component->second)->Data());
    }
    template <typename T>
    bool HasComponent() const
    {
        return component_list_.find(Component<T>::CLASS_INDEX) != component_list_.end();
    }
    template <typename T, typename V, typename ...Args>
    bool HasComponent() const
    {
        return HasComponent<T>() && HasComponent<V, Args...>();
    }
    template <typename T, typename ...Args>
    ComponentHandle<T> AddComponent(Args && ...args)
    {
        auto iter_component = component_list_.find(Component<T>::CLASS_INDEX);
        if (iter_component != component_list_.end())
        {
            Component<T> *component = static_cast<Component<T> *>(iter_component->second);
            *component = std::move(T(std::forward<Args>(args)...));
            return ComponentHandle<T>(component->Data());
        }
        Component<T> *component = new Component<T>(std::forward<Args>(args)...);
        component_list_[Component<T>::CLASS_INDEX] = component;
        return ComponentHandle<T>(component->Data());
    }
    template <typename T>
    void RemoveComponent()
    {
        auto iter_component = component_list_.find(Component<T>::CLASS_INDEX);
        if (iter_component == component_list_.end())
        {
            return;
        }
        delete iter_component->second;
        component_list_.erase(iter_component);
    }
    void RemoveAllComponent();
private:
    EntityID id_;
    std::map<ComponentBase::ClassIndexType, ComponentBase *> component_list_;
};

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager & operator=(const EntityManager &) = delete;
public:
    Entity * CreateEntity();
    void DestroyEntity(EntityID id);
    void DestroyAllEntity();
    Entity * GetEntity(EntityID id);
    template <typename T, typename ...Args>
    void ForeachEntity(std::function<void (Entity *)> cb)
    {
        if (!cb)
        {
            return;
        }
        for (auto &iter_entity : entity_list_)
        {
            if (!iter_entity.second->HasComponent<T, Args...>())
            {
                continue;
            }
            cb(iter_entity.second);
        }
    }
    void ForeachEntity(std::function<void (Entity *)> cb);
private:
    EntityID next_entity_id_;
    std::map<EntityID, Entity *> entity_list_;
};

class EntitySystemBase
{
public:
    using ClassIndexType = std::size_t;
public:
    EntitySystemBase();
    virtual ~EntitySystemBase() = 0;
    EntitySystemBase(const EntitySystemBase &) = delete;
    EntitySystemBase & operator=(const EntitySystemBase &) = delete;
public:
    bool Init();
    void Uninit();
    void Update(int64_t time_delta);
    bool Configure();
    void Unconfigure();
private:
    virtual bool _Init();
    virtual void _Uninit();
    virtual void _Update(int64_t time_delta);
    virtual bool _Configure();
    virtual void _Unconfigure();
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class EntitySystem
    : public EntitySystemBase
{
public:
    const static ClassIndexType CLASS_INDEX = ++EntitySystemBase::next_class_index_;
public:
    EntitySystem()
    {
    }
    virtual ~EntitySystem()
    {
    }
    EntitySystem(const EntitySystem &) = delete;
    EntitySystem & operator=(const EntitySystem &) = delete;
};

class EntitySystemManager
{
public:
    EntitySystemManager();
    ~EntitySystemManager();
    EntitySystemManager(const EntitySystemManager &) = delete;
    EntitySystemManager & operator=(const EntitySystemManager &) = delete;
public:
    template <typename T, typename ...Args>
    T * AddSystem(Args && ...args)
    {
        static_assert(std::is_base_of<EntitySystem<T>, T>::value, "system must be extern EntitySystem<T>");
        T *system = new T(std::forward<Args>(args)...);
        if (!system->Init())
        {
            delete system;
            return nullptr;
        }
        system_list_.push_back(system);
        return system;
    }
    bool Configure();
    void Unconfigure();
    void Update(int64_t time_delta);
private:
    std::list<EntitySystemBase *> system_list_;
};

}

#endif
