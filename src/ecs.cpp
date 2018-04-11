#include <mzx/ecs.h>

namespace mzx {

ComponentBase::ClassIndexType ComponentBase::next_class_index_ = 0;

Entity::Entity(EntityID id)
    : id_(id)
{
}

Entity::~Entity()
{
    RemoveAllComponent();
}

EntityID Entity::Id() const
{
    return id_;
}

void Entity::RemoveAllComponent()
{
    for (auto iter_component : component_list_)
    {
        delete iter_component.second;
    }
    component_list_.clear();
}

EntityManager::EntityManager()
    : next_entity_id_(0)
{
}

EntityManager::~EntityManager()
{
    DestroyAllEntity();
}

Entity * EntityManager::CreateEntity()
{
    Entity *entity = new Entity(++next_entity_id_);
    entity_list_[entity->Id()] = entity;
    return entity;
}

void EntityManager::DestroyEntity(EntityID id)
{
    auto iter_entity = entity_list_.find(id);
    if (iter_entity == entity_list_.end())
    {
        return;
    }
    delete iter_entity->second;
    entity_list_.erase(iter_entity);
}

void EntityManager::DestroyAllEntity()
{
    for (auto iter_entity : entity_list_)
    {
        delete iter_entity.second;
    }
    entity_list_.clear();
}

Entity * EntityManager::GetEntity(EntityID id)
{
    auto iter_entity = entity_list_.find(id);
    if (iter_entity == entity_list_.end())
    {
        return nullptr;
    }
    return iter_entity->second;
}

void EntityManager::ForeachEntity(const std::function<void (Entity *)> &cb)
{
    for (auto iter_entity : entity_list_)
    {
        cb(iter_entity.second);
    }
}

EntitySystemBase::ClassIndexType EntitySystemBase::next_class_index_ = 0;

EntitySystemManager::EntitySystemManager()
{

}

EntitySystemManager::~EntitySystemManager()
{
    RemoveAllSystem();
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

void EntitySystemManager::RemoveAllSystem()
{
    for (auto &system : system_list_)
    {
        delete system;
    }
    system_list_.clear();
}

}
