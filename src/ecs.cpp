#include <mzx/ecs.h>

namespace mzx
{

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

Entity *EntityManager::GetEntity(EntityID id) const
{
    auto iter_entity = entities_.find(id);
    if (iter_entity == entities_.end())
    {
        return nullptr;
    }
    return iter_entity->second->Get();
}

EntityManager::ComponentChangedEvent &EntityManager::ComponentAddEvent()
{
    return component_add_event_;
}

EntityManager::ComponentChangedEvent &EntityManager::ComponentRemoveEvent()
{
    return component_remove_event_;
}

EntityManager::EntityChangedEvent &EntityManager::EntityAddEvent()
{
    return entity_add_event_;
}

EntityManager::EntityChangedEvent &EntityManager::EntityRemoveEvent()
{
    return entity_remove_event_;
}

Entity *EntityManager::AddEntity()
{
    MZX_CHECK_STATIC(sizeof(Entity *) == sizeof(EventID));
    auto *entity = new Entity(*this);
    entity->id_ = reinterpret_cast<EntityID>(entity);
    auto *entity_node = new EntityNode(entity);
    entities_[entity->ID()] = entity_node;
    entity_list_.PushBack(&entity_node->list_link_);
    entity_node->IncrRef();
    entity_add_event_.Invoke(entity);
    entity = entity_node->Get();
    entity_node->DecrRef();
    return entity;
}

void EntityManager::RemoveEntity(EntityID id)
{
    auto iter_entity = entities_.find(id);
    if (iter_entity == entities_.end())
    {
        return;
    }
    auto *entity_node = iter_entity->second;
    entities_.erase(iter_entity);
    auto *entity = entity_node->Detach();
    if (entity)
    {
        entity->RemoveAllComponent();
        entity_remove_event_.Invoke(entity);
        delete entity;
    }
}

void EntityManager::RemoveAllEntity()
{
    for (auto *node = entity_list_.Begin(); node != entity_list_.End();)
    {
        auto *entity_node = MZX_CONTAINER_OF(node, EntityNode, list_link_);
        entity_node->IncrRef();
        auto *entity = entity_node->Detach();
        if (entity)
        {
            entities_.erase(entity->ID());
            entity->RemoveAllComponent();
            entity_remove_event_.Invoke(entity);
            delete entity;
        }
        node = node->Next();
        entity_node->DecrRef();
    }
}

void EntityManager::ForeachEntity(std::function<bool(Entity *)> cb)
{
    MZX_CHECK(cb != nullptr);
    for (auto *node = entity_list_.Begin(); node != entity_list_.End();)
    {
        auto *entity_node = MZX_CONTAINER_OF(node, EntityNode, list_link_);
        entity_node->IncrRef();
        if (entity_node->Get() && !cb(entity_node->Get()))
        {
            entity_node->DecrRef();
            return;
        }
        node = node->Next();
        entity_node->DecrRef();
    }
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

void Entity::ForeachComponent(std::function<bool(ComponentBase *)> cb)
{
    MZX_CHECK(cb != nullptr);
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

} // namespace mzx
