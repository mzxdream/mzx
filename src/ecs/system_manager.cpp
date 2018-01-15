#include <mzx/ecs/system_manager.h>
#include <mzx/ecs/entity_manager.h>
#include <mzx/ecs/event_manager.h>
#include <mzx/ecs/system.h>

namespace mzx {
namespace ecs {

SystemManager::SystemManager(EntityManager &entity_manager, EventManager &event_manager)
    : entity_manager_(entity_manager)
    , event_manager_(event_manager)
{
}

SystemManager::~SystemManager()
{
    RemoveAllSystem();
}

void SystemManager::RemoveSystem(System *system)
{
    for (auto iter_system = system_list_.begin(); iter_system != system_list_.end(); ++iter_system)
    {
        if (*iter_system == system)
        {
            system->Unconfigure(entity_manager_, event_manager_);
            delete system;
            system_list_.erase(iter_system);
            return;
        }
    }
}

void SystemManager::RemoveAllSystem()
{
    for (auto &iter_system : system_list_)
    {
        iter_system->Unconfigure(entity_manager_, event_manager_);
        delete iter_system;
    }
    system_list_.clear();
}

void SystemManager::Update(int64_t time_delta)
{
    for (auto &iter_system : system_list_)
    {
        iter_system->Update(entity_manager_, event_manager_, time_delta);
    }
}

}
}
