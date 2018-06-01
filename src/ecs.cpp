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

void Entity::SetAddComponentCallback(std::function<void (Entity *, ComponentBase *)> cb)
{
    add_component_cb_ = cb;
}

void Entity::SetRemoveComponentCallback(std::function<void (Entity *, ComponentBase *)> cb)
{
    remove_component_cb_ = cb;
}

void Entity::SetReplaceComponentCallback(std::function<void (Entity *, ComponentBase *)> cb)
{
    replace_component_cb_ = cb;
}

void Entity::RemoveAllComponent()
{
    for (auto iter_component : component_list_)
    {
        if (remove_component_cb_)
        {
            remove_component_cb_(this, iter_component.second);
        }
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

EntitySystemBase::ClassIndexType EntitySystemBase::next_class_index_ = 0;

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

void EntitySystemBase::Update(int64_t time_delta)
{
    _Update(time_delta);
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

void EntitySystemBase::_Update(int64_t time_delta)
{

}

bool EntitySystemBase::_Configure()
{
    return true;
}

void EntitySystemBase::_Unconfigure()
{

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

void EntitySystemManager::Update(int64_t time_delta)
{
    for (auto &system : system_list_)
    {
        system->Update(time_delta);
    }
}

}
