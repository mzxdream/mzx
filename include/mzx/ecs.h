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
    friend Entity;

public:
    using ClassIndexType = std::size_t;

protected:
    ComponentBase();
    ComponentBase(const ComponentBase &) = delete;
    ComponentBase &operator=(const ComponentBase &) = delete;
    virtual ~ComponentBase() = 0;

public:
    virtual ClassIndexType ClassIndex() const = 0;
    static ClassIndexType ClassIndexCount();

protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class Component : public ComponentBase
{
    friend Entity;

public:
    const static ClassIndexType CLASS_INDEX;

protected:
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

class EntityManager
{
    friend Entity;
    using EntityNode = typename ListSafeNode<Entity>;

public:
    using ComponentChangedEvent = Event<void(Entity *, ComponentBase *)>;
    using EntityChangedEvent = Event<void(Entity *)>;

public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;

public:
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

class Entity
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
    T *AddComponent(Args &&... args)
    {
        MZX_CHECK(component_list_[Component<T>::CLASS_INDEX] == nullptr);
        auto *component = new Component<T>(std::forward<Args>(args)...);
        component_list_[Component<T>::CLASS_INDEX] = component;
        entity_manager_.OnAddComponent(this, component);
        return component->Get();
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
    void ForeachComponent(std::function<bool(ComponentBase *)> cb);

private:
    EntityID id_{0};
    EntityManager &entity_manager_;
    std::vector<ComponentBase *> component_list_;
};

class EntitySystemBase
{
public:
    using ClassIndexType = std::size_t;

public:
    EntitySystemBase();
    virtual ~EntitySystemBase() = 0;
    EntitySystemBase(const EntitySystemBase &) = delete;
    EntitySystemBase &operator=(const EntitySystemBase &) = delete;

public:
    bool Init();
    void Uninit();
    void Update();

private:
    virtual bool _Init();
    virtual void _Uninit();
    virtual void _Update();

public:
    static ClassIndexType ClassIndexCount();
    virtual ClassIndexType ClassIndex() const = 0;

protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class EntitySystem : public EntitySystemBase
{
public:
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

template <typename T>
const EntitySystemBase::ClassIndexType EntitySystem<T>::CLASS_INDEX =
    EntitySystemBase::class_index_counter_++;

class EntitySystemManager
{
    using SystemNode = ListSafeNode<EntitySystemBase>;

public:
    EntitySystemManager();
    ~EntitySystemManager();
    EntitySystemManager(const EntitySystemManager &) = delete;
    EntitySystemManager &operator=(const EntitySystemManager &) = delete;

public:
    template <typename T, typename... Args>
    T *AddSystem(Args &&... args)
    {
        MZX_CHECK_STATIC(std::is_base_of<EntitySystem<T>, T>::value);
        MZX_CHECK(systems_[T::CLASS_INDEX] == nullptr);
        auto *system = new T(std::forward<Args>(args)...);
        if (!system->Init())
        {
            delete system;
            return nullptr;
        }
        auto *system_node = new SystemNode(system);
        system_list_.PushBack(&system_node->list_link_);
        systems_[T::CLASS_INDEX] = node;
        return system;
    }
    template <typename T>
    void RemoveSystem()
    {
        MZX_CHECK_STATIC(std::is_base_of<EntitySystem<T>, T>::value);
        auto *system_node = systems_[T::CLASS_INDEX];
        if (!system_node)
        {
            return;
        }
        systems_[T::CLASS_INDEX] = nullptr;
        auto *system = system_node->Detach();
        if (system)
        {
            system->Uninit();
            delete system;
        }
    }
    void RemoveAllSystem();
    template <typename T>
    void Update()
    {
        MZX_CHECK_STATIC(std::is_base_of<EntitySystem<T>, T>::value);
        auto *system_node = systems_[T::CLASS_INDEX];
        if (system_node && system_node->Get())
        {
            system_node->Get()->Update();
        }
    }
    void UpdateAll();

private:
    std::vector<SystemNode *> systems_;
    List system_list_;
};

} // namespace mzx

#endif
