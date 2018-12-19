#ifndef __MZX_ECS_H__
#define __MZX_ECS_H__

#include <list>
#include <vector>
#include <cstddef>
#include <utility>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <mzx/event.h>
#include <mzx/logger.h>

namespace mzx {

class Entity;

class ComponentBase
{
    friend Entity;
public:
    using ClassIndexType = std::size_t;
protected:
    ComponentBase();
    ComponentBase(const ComponentBase &) = delete;
    ComponentBase & operator=(const ComponentBase &) = delete;
    virtual ~ComponentBase() = 0;
public:
    virtual ClassIndexType ClassIndex() = 0;
    static ClassIndexType ClassIndexCount();
protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class Component
    : public ComponentBase
{
    friend Entity;
public:
    const static ClassIndexType CLASS_INDEX = ComponentBase::class_index_counter_++;
protected:
    template <typename ...Args>
    explicit Component(Args ...args)
        : raw_data_(std::forward<Args>(args)...)
    {
    }
    virtual ~Component()
    {
    }
    Component(const Component &) = delete;
    Component & operator=(const Component &) = delete;
public:
    T & Get()
    {
        return raw_data_;
    }
    T * GetPtr()
    {
        return &raw_data_;
    }
    virtual ClassIndexType ClassIndex() override
    {
        return CLASS_INDEX;
    }
private:
    T raw_data_;
};

using EntityID = int64_t;
constexpr EntityID ENTITY_ID_INVALID = (EntityID)-1;

class EntityManager
{
    friend Entity;
public:
    using ComponentChangedEvent = Event<void (Entity *, ComponentBase *)>;
    using EntityChangedEvent = Event<void (Entity *)>;
public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager & operator=(const EntityManager &) = delete;
public:
    ComponentChangedEvent & ComponentAddEvent();
    ComponentChangedEvent & ComponentRemoveEvent();
    EntityChangedEvent & EntityAddEvent();
    EntityChangedEvent & EntityRemoveEvent();

    Entity * GetEntity(EntityID id);
    Entity * AddEntity();
    void RemoveEntity(EntityID id);
    void RemoveAllEntity();
    void ForeachEntity(std::function<bool (Entity *)> cb);
private:
    void OnAddComponent(Entity *, ComponentBase *);
    void OnRemoveComponent(Entity *, ComponentBase *);
private:
    ComponentChangedEvent component_add_event_;
    ComponentChangedEvent component_remove_event_;
    EntityChangedEvent entity_add_event_;
    EntityChangedEvent entity_remove_event_;
    std::unordered_map<EntityID, Entity *> entities_;
    ListHead entity_list_;
};

class Entity
{
    friend EntityManager;
private:
    Entity(EntityManager &entity_manager);
    ~Entity();
    Entity(const Entity &) = delete;
    Entity & operator=(const Entity &) = delete;
public:
    EntityID ID() const;
    template <typename T>
    Component<T> * GetComponent() const
    {
        return static_cast<Component<T> *>(component_list_[Component<T>::CLASS_INDEX]);
    }
    template <typename T>
    bool HasComponent() const
    {
        return component_list_[Component<T>::CLASS_INDEX] != nullptr;
    }
    template <typename T, typename V, typename ...Args>
    bool HasComponent() const
    {
        return HasComponent<T>() && HasComponent<V, Args...>();
    }
    template <typename T, typename ...Args>
    Component<T> * AddComponent(Args ...args)
    {
        MZX_CHECK(component_list_[Component<T>::CLASS_INDEX] == nullptr);
        auto *component = new Component<T>(std::forward<Args>(args)...);
        component_list_[Component<T>::CLASS_INDEX] = component;
        entity_manager_.OnAddComponent(this, component);
        return component->GetPtr();
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
    void ForeachComponent(std::function<bool (ComponentBase *)> cb);
private:
    void SetID(EntityID id);
    void IncrRef();
    void DecrRef();
    void SelfRemove();
private:
    EntityID id_{ 0 };
    EntityManager &entity_manager_;
    std::vector<ComponentBase *> component_list_;
    bool invalid_{ false };
    int ref_count_{ 0 };
    ListHead list_link_;
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
    void Update();
private:
    virtual bool _Init();
    virtual void _Uninit();
    virtual void _Update();
public:
    static ClassIndexType ClassIndexCount();
    virtual ClassIndexType ClassIndex() = 0;
protected:
    static ClassIndexType class_index_counter_;
};

template <typename T>
class EntitySystem
    : public EntitySystemBase
{
public:
    const static ClassIndexType CLASS_INDEX = EntitySystemBase::class_index_counter_++;
public:
    EntitySystem()
    {
    }
    virtual ~EntitySystem()
    {
    }
    EntitySystem(const EntitySystem &) = delete;
    EntitySystem & operator=(const EntitySystem &) = delete;
public:
    virtual ClassIndexType ClassIndex() override
    {
        return CLASS_INDEX;
    }
};

class EntitySystemManager
{
    struct SystemNode
    {
        SystemNode(EntitySystemBase *sys)
            : system(sys)
            , ref_count(1)
        {
            MZX_CHECK(sys != nullptr);
            MZX_INIT_LIST_HEAD(&list_link);
        }
        ~SystemNode()
        {
            MZX_CHECK(system == nullptr && ref_count == 0);
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
            if (system != nullptr)
            {
                system->Uninit();
                delete system;
                system = nullptr;
                DecrRef();
            }
        }
        EntitySystemBase *system{ nullptr };
        int ref_count{ 0 };
        ListHead list_link;
    };
public:
    EntitySystemManager();
    ~EntitySystemManager();
    EntitySystemManager(const EntitySystemManager &) = delete;
    EntitySystemManager & operator=(const EntitySystemManager &) = delete;
public:
    template <typename T, typename ...Args>
    T * AddSystem(Args ...args)
    {
        MZX_CHECK_STATIC((std::is_base_of<T, int>::value));
        MZX_CHECK(system_index_[T::CLASS_INDEX] == nullptr);
        auto *system = new T(std::forward<Args>(args)...);
        if (!system->Init())
        {
            delete system;
            return nullptr;
        }
        auto *node = new SystemNode(system);
        MZX_LIST_PUSH_BACK(&node->list_link, &system_list_);
        system_list_[T::CLASS_INDEX] = system;
        return system;
    }
    template <typename T>
    void RemoveSystem()
    {
        MZX_CHECK_STATIC((std::is_base_of<EntitySystem<T>, T>::value));
        auto *node = system_list_[T::CLASS_INDEX];
        if (node)
        {
            system_list_[T::CLASS_INDEX] = nullptr;
            node->SelfRemove();
        }
    }
    void RemoveAllSystem();
    template <typename T>
    void Update()
    {
        MZX_CHECK_STATIC((std::is_base_of<EntitySystem<T>, T>::value));
        auto *node = system_list_[T::CLASS_INDEX];
        if (node && node->system)
        {
            node->system->Update();
        }
    }
    void UpdateAll();
private:
    ListHead system_list_;
    std::vector<SystemNode *> system_index_;
};

}

#endif
