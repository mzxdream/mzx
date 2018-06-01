#include <mzx/ecs.h>

namespace mzx {

ComponentBase::ClassIndexType ComponentBase::class_index_counter_ = 0;

ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}

ComponentBase::ClassIndexType ComponentBase::ClassIndexCount()
{
    return class_index_counter_;
}

EntityManager::EntityManager()
    : next_entity_id_(0)
{
}

EntityManager::~EntityManager()
{
    RemoveAllEntity();
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

Entity * EntityManager::AddEntity()
{
    Entity *entity = new Entity(++next_entity_id_, *this);
    entity_list_[entity->Id()] = entity;
    add_entity_event_.Invoke(entity);
    return entity;
}

void EntityManager::RemoveEntity(EntityID id)
{
    auto iter_entity = entity_list_.find(id);
    if (iter_entity == entity_list_.end())
    {
        return;
    }
    remove_entity_event_.Invoke(iter_entity->second);
    delete iter_entity->second;
    entity_list_.erase(iter_entity);
}

void EntityManager::RemoveAllEntity()
{
    for (auto iter_entity : entity_list_)
    {
        remove_entity_event_.Invoke(iter_entity.second);
        delete iter_entity.second;
    }
    entity_list_.clear();
}

void EntityManager::ForeachEntity(std::function<void (Entity *)> cb)
{
    if (!cb)
    {
        return;
    }
    for (auto iter_entity : entity_list_)
    {
        cb(iter_entity.second);
    }
}

Event<void (Entity *, ComponentBase *)> & EntityManager::AddComponentEvent()
{
    return add_component_event_;
}

Event<void (Entity *, ComponentBase *)> & EntityManager::RemoveComponentEvent()
{
    return remove_component_event_;
}

Event<void (Entity *, ComponentBase *)> & EntityManager::ReplaceComponentEvent()
{
    return replace_component_event_;
}

Event<void (Entity *)> & EntityManager::AddEntityEvent()
{
    return add_entity_event_;
}

Event<void (Entity *)> & EntityManager::RemoveEntityEvent()
{
    return remove_entity_event_;
}

Entity::Entity(EntityID id, EntityManager &entity_manager)
    : id_(id)
    , entity_manager_(entity_manager)
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
        entity_manager_.OnRemoveComponent(this, iter_component.second);
        delete iter_component.second;
    }
    component_list_.clear();
}

EntitySystemBase::ClassIndexType EntitySystemBase::class_index_counter_ = 0;

EntitySystemBase::EntitySystemBase()
{

}

EntitySystemBase::~EntitySystemBase()
{

}

bool EntitySystemBase::Init()
{
    return _Init();
}

void EntitySystemBase::Uninit()
{
    _Uninit();
}

void EntitySystemBase::Update(int64_t now_time)
{
    _Update(now_time);
}

bool EntitySystemBase::Configure()
{
    return _Configure();
}

void EntitySystemBase::Unconfigure()
{
    _Unconfigure();
}

bool EntitySystemBase::_Init()
{
    return true;
}

void EntitySystemBase::_Uninit()
{

}

void EntitySystemBase::_Update(int64_t now_time)
{

}

bool EntitySystemBase::_Configure()
{
    return true;
}

void EntitySystemBase::_Unconfigure()
{

}

EntitySystemBase::ClassIndexType EntitySystemBase::ClassIndexCount()
{
    return class_index_counter_;
}

EntitySystemManager::EntitySystemManager()
{

}

EntitySystemManager::~EntitySystemManager()
{
    for (auto &system : system_list_)
    {
        system->Uninit();
        delete system;
    }
    system_list_.clear();
}

bool EntitySystemManager::Configure()
{
    for (auto &system : system_list_)
    {
        if (!system->Configure())
        {
            return false;
        }
    }
    return true;
}

void EntitySystemManager::Unconfigure()
{
    for (auto &system : system_list_)
    {
        system->Unconfigure();
    }
}

void EntitySystemManager::Update(int64_t now_time)
{
    for (auto &system : system_list_)
    {
        system->Update(now_time);
    }
}

}
