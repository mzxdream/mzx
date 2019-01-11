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
{
}

EntityManager::~EntityManager()
{
    RemoveAllEntity();
}

Entity * EntityManager::GetEntity(EntityID id)
{
    auto iter_entity = entities_.find(id);
    if (iter_entity == entities_.end())
    {
        return nullptr;
    }
    return iter_entity->second->entity;
}

Entity * EntityManager::AddEntity()
{
    MZX_CHECK_STATIC(sizeof(Entity *) == sizeof(EventID));
    auto *entity = new Entity(*this);
    entity->SetID((EntityID)entity);
    auto *node = new EntityNode(entity);
    entity_list_.PushBack(&node->list_link);
    entities_[entity->ID()] = node;
    node->IncrRef();
    entity_add_event_.Invoke(node->entity);
    entity = node->entity;
    node->DecrRef();
    return entity;
}

void EntityManager::RemoveEntity(EntityID id)
{
    auto iter_entity = entities_.find(id);
    if (iter_entity == entities_.end())
    {
        return;
    }
    auto *node = iter_entity->second;
    entities_.erase(iter_entity);
    auto *entity = node->DetachEntity();
    if (entity)
    {
        entity->RemoveAllComponent();
        entity_remove_event_.Invoke(entity);
        delete entity;
    }
}

void EntityManager::RemoveAllEntity()
{
    for (auto *node = entity_list_.Next(); node != &entity_list_; node = entity_list_.Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, EntityNode, list_link);
        auto *entity = entry->DetachEntity();
        if (entity)
        {
            entities_.erase(entity->ID());
            entity->RemoveAllComponent();
            entity_remove_event_.Invoke(entity);
            delete entity;
        }
    }
}

void EntityManager::ForeachEntity(std::function<bool (Entity *)> cb)
{
    MZX_CHECK(cb != nullptr);
    for (auto *node = entity_list_.Next(); node != &entity_list_; )
    {
        auto *entry = MZX_LIST_ENTRY(node, EntityNode, list_link);
        if (!entry->entity)
        {
            node = node->Next();
            continue;
        }
        entry->IncrRef();
        if (!cb(entry->entity))
        {
            entry->DecrRef();
            return;
        }
        node = node->Next();
        entry->DecrRef();
    }
}

EntityManager::ComponentChangedEvent & EntityManager::ComponentAddEvent()
{
    return component_add_event_;
}

EntityManager::ComponentChangedEvent & EntityManager::ComponentRemoveEvent()
{
    return component_remove_event_;
}

EntityManager::EntityChangedEvent & EntityManager::EntityAddEvent()
{
    return entity_add_event_;
}

EntityManager::EntityChangedEvent & EntityManager::EntityRemoveEvent()
{
    return entity_remove_event_;
}

void EntityManager::OnAddComponent(Entity *entity, ComponentBase *component)
{
    component_add_event_.Invoke(entity, component);
}

void EntityManager::OnRemoveComponent(Entity *entity, ComponentBase *component)
{
    component_remove_event_.Invoke(entity, component);
}

Entity::Entity(EntityManager &entity_manager)
    : entity_manager_(entity_manager)
    , component_list_(ComponentBase::ClassIndexCount())
{
}

Entity::~Entity()
{
    RemoveAllComponent();
}

void Entity::SetID(EntityID id)
{
    id_ = id;
}

EntityID Entity::ID() const
{
    return id_;
}

void Entity::RemoveAllComponent()
{
    for (std::size_t i = 0; i < component_list_.size(); ++i)
    {
        auto *component = component_list_[i];
        if (component)
        {
            component_list_[i] = nullptr;
            entity_manager_.OnRemoveComponent(this, component);
            delete component;
        }
    }
}

void Entity::ForeachComponent(std::function<bool (ComponentBase *)> cb)
{
    if (!cb)
    {
        return;
    }
    for (std::size_t i = 0; i < component_list_.size(); ++i)
    {
        auto *component = component_list_[i];
        if (!component)
        {
            continue;
        }
        if (!cb(component))
        {
            return;
        }
    }
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

void EntitySystemBase::Update()
{
    _Update();
}

bool EntitySystemBase::_Init()
{
    return true;
}

void EntitySystemBase::_Uninit()
{

}

void EntitySystemBase::_Update()
{

}

EntitySystemBase::ClassIndexType EntitySystemBase::ClassIndexCount()
{
    return class_index_counter_;
}

EntitySystemManager::EntitySystemManager()
    : systems_(EntitySystemBase::ClassIndexCount())
{
}

EntitySystemManager::~EntitySystemManager()
{
    RemoveAllSystem();
}

void EntitySystemManager::RemoveAllSystem()
{
    for (auto *node = system_list_.Next(); node != &system_list_; node = system_list_.Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, SystemNode, list_link);
        if (entry->system)
        {
            systems_[entry->system->ClassIndex()] = nullptr;
        }
        entry->SelfRemove();
    }
}

void EntitySystemManager::UpdateAll()
{
    for (auto *node = system_list_.Next(); node != &system_list_; )
    {
        auto *entry = MZX_LIST_ENTRY(node, SystemNode, list_link);
        entry->IncrRef();
        if (entry->system)
        {
            entry->system->Update();
        }
        node = node->Next();
        entry->DecrRef();
    }
}

}
