#ifndef __MZX_ENTITY_H__
#define __MZX_ENTITY_H__

#include <cstddef>
#include <map>

#include <mzx/ecs/component.h>

namespace mzx {

class Entity
{
public:
    typedef std::size_t ID;
    const static ID ID_INVALID = (ID)-1;
public:
    Entity(ID id);
    ~Entity();
    Entity(const Entity &) = delete;
    Entity & operator=(const Entity &) = delete;
public:
    ID Id() const;
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
    ID id_;
    std::map<ComponentBase::ClassIndexType, ComponentBase *> component_list_;
};

}

#endif
