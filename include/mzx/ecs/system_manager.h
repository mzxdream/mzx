#ifndef __MZX_ECS_SYSTEM_MANAGER_H__
#define __MZX_ECS_SYSTEM_MANAGER_H__

#include <cstdint>
#include <utility>
#include <map>

#include <mzx/ecs/system.h>

namespace mzx {
namespace ecs {

class EntityManager;
class EventManager;

class SystemManager
{
public:
    SystemManager(EntityManager &entity_manager, EventManager &event_manager);
    ~SystemManager();
    SystemManager(const SystemManager &) = delete;
    SystemManager& operator=(const SystemManager &) = delete;
public:
    template <typename T>
    T * GetSystem() const
    {
        auto iter_system = system_list_.find(T::CLASS_INDEX);
        if (iter_system == system_list_.end())
        {
            return nullptr;
        }
        return static_cast<T *>(iter_system->second);
    }
    template <typename T, typename ...Args>
    T * AddSystem(Args && ...args)
    {
        auto iter_system = system_list_.find(T::CLASS_INDEX);
        if (iter_system != system_list_.end())
        {
            return nullptr;
        }
        T *system = new T(std::forward(args)...);
        system_list_[T::CLASS_INDEX] = system;
        system->Configure(entity_manager_, event_manager_);
        return system;
    }
    template <typename T>
    void RemoveSystem()
    {
        auto iter_system = system_list_.find(T::CLASS_INDEX);
        if (iter_system == system_list_.end())
        {
            return;
        }
        auto system = iter_system->second;
        system->Unconfigure(entity_manager_, event_manager_);
        delete system;
        system_list_.erase(iter_system);
    }
    void RemoveAllSystem();
    template <typename T>
    void Update(int64_t time_delta)
    {
        auto iter_system = system_list_.find(T::CLASS_INDEX);
        if (iter_system == system_list_.end())
        {
            return;
        }
        iter_system->second->Update(time_delta);
    }
    void UpdateAll(int64_t time_delta);
private:
    EntityManager &entity_manager_;
    EventManager &event_manager_;
    std::map<SystemBase::ClassIndexType, SystemBase *> system_list_;
};

}
}

#endif
