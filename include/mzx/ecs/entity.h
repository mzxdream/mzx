#ifndef __MZX_ECS_ENTITY_H__
#define __MZX_ECS_ENTITY_H__

#include <cstddef>
#include <map>

#include <mzx/ecs/component.h>

namespace mzx {
namespace ecs {

class Entity
{
public:
    typedef std::size_t ID;
public:
    Entity(ID id);
    ~Entity();
    Entity(const Entity &) = delete;
    Entity & operator=(const Entity &) = delete;
public:
    ID GetID() const;
    template <typename T>
    ComponentHandler<T> GetComponent() const
    {
        auto iter_component = component_list_.find(Component<T>::GetID());
        if (iter_component == component_list_.end())
        {
            return ComponentHandler<T>(nullptr);
        }
        return ComponentHandler<T>((static_cast<Component<T> *>(iter_component->second))->Data());
    }
    template <typename T>
    bool HasComponent() const
    {
        return component_list_.find(Component<T>::GetID()) != component_list_.end();
    }
    template <typename T, typename V, typename ...Args>
    bool HasComponent() const
    {
        return HasComponent<T>() && HasComponent<V, Args...>();
    }
    template <typename T, typename ...Args>
    ComponentHandler<T> AddComponent(Args && ...args)
    {
        auto iter_component = component_list_.find(Component<T>::GetID());
        if (iter_component != component_list_.end())
        {
            Component<T> *component = static_cast<Component<T> *>(iter_component->second);
            *(component->Data()) = T(std::forward<Args>(args)...);
            return ComponentHandler<T>(component->Data());
        }
        Component<T> *component = new Component<T>(std::forward<Args>(args)...);
        component_list_[Component<T>::GetID()] = component;
        return ComponentHandler<T>(component->Data());
    }
    template <typename T>
    void RemoveComponent()
    {
        auto iter_component = component_list_.find(Component<T>::GetID());
        if (iter_component == component_list_.end())
        {
            return;
        }
        delete iter_component.second;
        component_list_.erase(iter_component);
    }
private:
    ID id_;
    std::map<ComponentBase::ID, ComponentBase *> component_list_;
};

}
}

#endif
