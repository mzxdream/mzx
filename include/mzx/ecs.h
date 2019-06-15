#ifndef __MZX_ECS_H__
#define __MZX_ECS_H__

#include <functional>
#include <unordered_map>
#include <vector>

#include <mzx/data_structure/list_safe_node.h>
#include <mzx/event.h>
#include <mzx/logger.h>
#include <mzx/macro_const_overload.h>

namespace mzx
{

class Entity;

class ComponentBase
{
public:
    using ClassIndexType = std::size_t;

    ComponentBase();
    ComponentBase(const ComponentBase &) = delete;
    ComponentBase &operator=(const ComponentBase &) = delete;
    virtual ~ComponentBase() = 0;

public:
    virtual ComponentBase *Clone() const = 0;
    virtual ClassIndexType ClassIndex() const = 0;
    static ClassIndexType ClassIndexCount();

protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class Component final : public ComponentBase
{
public:
    const static ClassIndexType CLASS_INDEX;

    template <typename... Args>
    explicit Component(Args &&... args)
        : raw_data_(std::forward<Args>(args)...)
    {
    }
    virtual ~Component()
    {
    }
    Component(const Component &) = delete;
    Component &operator=(const Component &) = delete;

public:
    const T *Get() const
    {
        return &raw_data_;
    }
    MZX_NON_CONST_OVERLOAD(Get);
    virtual ComponentBase *Clone() const
    {
        return new Component<T>(raw_data_);
    }
    virtual ClassIndexType ClassIndex() const override
    {
        return CLASS_INDEX;
    }

private:
    T raw_data_;
};

template <typename T>
const ComponentBase::ClassIndexType Component<T>::CLASS_INDEX =
    ComponentBase::class_index_counter_++;

using EntityID = std::size_t;
constexpr EntityID ENTITY_ID_INVALID = (EntityID)-1;

class EntityManager final
{
    friend Entity;
    using EntityNode = ListSafeNode<Entity>;

public:
    using ComponentChangedEvent = Event<void(Entity *, ComponentBase *)>;
    using EntityChangedEvent = Event<void(Entity *)>;

public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;

public:
    std::size_t EntityCount() const;
    Entity *GetEntity(EntityID id) const;
    ComponentChangedEvent &ComponentAddEvent();
    ComponentChangedEvent &ComponentRemoveEvent();
    EntityChangedEvent &EntityAddEvent();
    EntityChangedEvent &EntityRemoveEvent();

    Entity *AddEntity();
    void RemoveEntity(EntityID id);
    void RemoveAllEntity();
    void ForeachEntity(std::function<bool(Entity *)> cb);

private:
    void OnAddComponent(Entity *, ComponentBase *);
    void OnRemoveComponent(Entity *, ComponentBase *);

private:
    ComponentChangedEvent component_add_event_;
    ComponentChangedEvent component_remove_event_;
    EntityChangedEvent entity_add_event_;
    EntityChangedEvent entity_remove_event_;
    std::unordered_map<EntityID, EntityNode *> entities_;
    List entity_list_;
};

class Entity final
{
    friend EntityManager;

private:
    explicit Entity(EntityManager &entity_manager);
    ~Entity();
    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

public:
    EntityID ID() const;
    template <typename T>
    T *GetComponent() const
    {
        auto *component = component_list_[Component<T>::CLASS_INDEX];
        if (!component)
        {
            return nullptr;
        }
        return static_cast<Component<T> *>(component)->Get();
    }
    template <typename T>
    bool HasComponent() const
    {
        return component_list_[Component<T>::CLASS_INDEX] != nullptr;
    }
    template <typename T, typename V, typename... Args>
    bool HasComponent() const
    {
        return HasComponent<T>() && HasComponent<V, Args...>();
    }
    template <typename T, typename... Args>
    Component<T> *AddComponent(Args &&... args)
    {
        MZX_CHECK(component_list_[Component<T>::CLASS_INDEX] == nullptr);
        auto *component = new Component<T>(std::forward<Args>(args)...);
        component_list_[Component<T>::CLASS_INDEX] = component;
        entity_manager_.OnAddComponent(this, component);
        return component == component_list_[Component<T>::CLASS_INDEX]
                   ? component
                   : nullptr;
    }
    template <typename T>
    void RemoveComponent()
    {
        auto *component = component_list_[Component<T>::CLASS_INDEX];
        if (component != nullptr)
        {
            component_list_[Component<T>::CLASS_INDEX] = nullptr;
            entity_manager_.OnRemoveComponent(this, component);
            delete component;
        }
    }
    void RemoveAllComponent();
    ComponentBase *CopyComponent(const ComponentBase *component_image)
    {
        MZX_CHECK(component_image != nullptr);
        MZX_CHECK(component_list_[component_image->ClassIndex()] == nullptr);
        auto *component = component_image->Clone();
        auto class_index = component->ClassIndex();
        component_list_[class_index] = component;
        entity_manager_.OnAddComponent(this, component);
        return component == component_list_[class_index] ? component : nullptr;
    }
    bool AttachComponent(ComponentBase *component)
    {
        MZX_CHECK(component != nullptr);
        MZX_CHECK(component_list_[component->ClassIndex()] == nullptr);
        auto class_index = component->ClassIndex();
        component_list_[class_index] = component;
        entity_manager_.OnAddComponent(this, component);
        return component == component_list_[class_index] ? component : nullptr;
    }
    template <typename T>
    Component<T> *DetachComponent()
    {
        auto *component = component_list_[Component<T>::CLASS_INDEX];
        if (!component)
        {
            return nullptr;
        }
        component_list_[Component<T>::CLASS_INDEX] = nullptr;
        entity_manager_.OnRemoveComponent(this, component);
        return static_cast<Component<T> *>(component);
    }
    void ForeachComponent(std::function<bool(ComponentBase *)> cb);

private:
    EntityID id_{0};
    EntityManager &entity_manager_;
    std::vector<ComponentBase *> component_list_;
};

template <typename R>
class EntitySystemBase;

template <typename R, typename... Args>
class EntitySystemBase<R(Args...)>
{
public:
    using ClassIndexType = std::size_t;

public:
    EntitySystemBase()
    {
    }
    virtual ~EntitySystemBase()
    {
    }
    EntitySystemBase(const EntitySystemBase &) = delete;
    EntitySystemBase &operator=(const EntitySystemBase &) = delete;

public:
    R Update(Args... args)
    {
        return _Update(args...);
    }

private:
    virtual R _Update(Args... args) = 0;

public:
    static ClassIndexType ClassIndexCount()
    {
        return class_index_counter_;
    }

    virtual ClassIndexType ClassIndex() const = 0;

protected:
    static ClassIndexType class_index_counter_;
};

template <typename R, typename... Args>
typename EntitySystemBase<R(Args...)>::ClassIndexType
    EntitySystemBase<R(Args...)>::class_index_counter_ = 0;

template <typename T, typename R>
class EntitySystem;

template <typename T, typename R, typename... Args>
class EntitySystem<T, R(Args...)> : public EntitySystemBase<R(Args...)>
{
public:
    using ClassIndexType =
        typename EntitySystemBase<R(Args...)>::ClassIndexType;
    const static ClassIndexType CLASS_INDEX;

public:
    EntitySystem()
    {
    }
    virtual ~EntitySystem()
    {
    }
    EntitySystem(const EntitySystem &) = delete;
    EntitySystem &operator=(const EntitySystem &) = delete;

public:
    virtual ClassIndexType ClassIndex() const override
    {
        return CLASS_INDEX;
    }
};

template <typename T, typename R, typename... Args>
const typename EntitySystem<T, R(Args...)>::ClassIndexType
    EntitySystem<T, R(Args...)>::CLASS_INDEX =
        EntitySystemBase<R(Args...)>::class_index_counter_++;

template <typename R>
class EntitySystemManager;

template <typename R, typename... Args>
class EntitySystemManager<R(Args...)> final
{
    using SystemNode = ListSafeNode<EntitySystemBase<R(Args...)>>;

public:
    EntitySystemManager()
        : systems_(EntitySystemBase<R(Args...)>::ClassIndexCount())
    {
    }
    ~EntitySystemManager()
    {
        RemoveAllSystem();
    }
    EntitySystemManager(const EntitySystemManager &) = delete;
    EntitySystemManager &operator=(const EntitySystemManager &) = delete;

public:
    template <typename T>
    T *AddSystem()
    {
        MZX_CHECK_STATIC(
            std::is_base_of<EntitySystem<T, R(Args...)>, T>::value);
        MZX_CHECK(systems_[T::CLASS_INDEX] == nullptr);
        auto *system_node = new SystemNode(new T());
        system_list_.PushBack(&system_node->list_link_);
        systems_[T::CLASS_INDEX] = system_node;
        return system_node->Get();
    }
    template <typename T>
    void RemoveSystem()
    {
        MZX_CHECK_STATIC(
            std::is_base_of<EntitySystem<T, R(Args...)>, T>::value);
        auto *system_node = systems_[T::CLASS_INDEX];
        if (!system_node)
        {
            return;
        }
        systems_[T::CLASS_INDEX] = nullptr;
        delete system_node->Detach();
    }
    void RemoveAllSystem()
    {
        for (auto *node = system_list_.Begin(); node != system_list_.End();)
        {
            auto *system_node = MZX_CONTAINER_OF(node, SystemNode, list_link_);
            system_node->IncrRef();
            auto *system = system_node->Detach();
            if (system)
            {
                systems_[system->ClassIndex()] = nullptr;
                delete system;
            }
            node = node->Next();
            system_node->DecrRef();
        }
    }
    template <typename T>
    void Update(Args... args)
    {
        MZX_CHECK_STATIC(
            std::is_base_of<EntitySystem<T, R(Args...)>, T>::value);
        auto *system_node = systems_[T::CLASS_INDEX];
        if (system_node && system_node->Get())
        {
            system_node->Get()->Update(args...);
        }
    }
    void UpdateAll(Args... args)
    {
        for (auto *node = system_list_.Begin(); node != system_list_.End();)
        {
            auto *system_node = MZX_CONTAINER_OF(node, SystemNode, list_link_);
            system_node->IncrRef();
            if (system_node->Get())
            {
                system_node->Get()->Update(args...);
            }
            node = node->Next();
            system_node->DecrRef();
        }
    }

private:
    std::vector<SystemNode *> systems_;
    List system_list_;
};

} // namespace mzx

#endif
