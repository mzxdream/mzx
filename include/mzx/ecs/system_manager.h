#ifndef __MZX_ECS_SYSTEM_MANAGER_H__
#define __MZX_ECS_SYSTEM_MANAGER_H__

#include <vector>
#include <cstdint>
#include <utility>

namespace mzx {
namespace ecs {

class EntityManager;
class EventManager;
class System;

class SystemManager
{
public:
    SystemManager(EntityManager &entity_manager, EventManager &event_manager);
    ~SystemManager();
    SystemManager(const SystemManager &) = delete;
    SystemManager& operator=(const SystemManager &) = delete;
public:
    template <typename T, typename ...Args>
    T * AddSystem(Args && ...args)
    {
        T *system = new T(std::forward(args)...);
        system_list_.push_back(system);
        system->Configure(entity_manager_, event_manager_);
        return system;
    }
    void RemoveSystem(System *system);
    void RemoveAllSystem();
    void Update(int64_t time_delta);
private:
    EntityManager &entity_manager_;
    EventManager &event_manager_;
    std::vector<System *> system_list_;
};

}
}

#endif
