#include <mzx/entity_system.h>

namespace mzx {

EntitySystemBase::ClassIndexType EntitySystemBase::next_class_index_ = 0;

EntitySystemManager::EntitySystemManager()
{

}

EntitySystemManager::~EntitySystemManager()
{

}

bool EntitySystemManager::InitAllSystem()
{
    for (auto &system : system_list_)
    {
        if (!system->Init())
        {
            return false;
        }
    }
    return true;
}

void EntitySystemManager::UninitAllSystem()
{
    for (auto &system : system_list_)
    {
        system->Uninit();
    }
}

void EntitySystemManager::UpdateAllSystem(int64_t time_delta)
{
    for (auto &system : system_list_)
    {
        system->Update(time_delta);
    }
}

void EntitySystemManager::ClearAllSystem()
{
    for (auto &system : system_list_)
    {
        delete system;
    }
    system_list_.clear();
}

}
