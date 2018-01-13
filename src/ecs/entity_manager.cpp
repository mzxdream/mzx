#include <mzx/ecs/entity_manager.h>

namespace mzx {
namespace ecs {

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
    entity_list_[entity->ID()] = entity;
    return entity;
}

void EntityManager::DestroyEntity(Entity::IDType id)
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

Entity * EntityManager::GetEntity(Entity::IDType id)
{
    auto iter_entity = entity_list_.find(id);
    if (iter_entity == entity_list_.end())
    {
        return nullptr;
    }
    return iter_entity->second;
}

void EntityManager::ForeachAllEntity(const std::function<void (Entity *)> &cb)
{
    for (auto iter_entity : entity_list_)
    {
        cb(iter_entity.second);
    }
}

}
}
