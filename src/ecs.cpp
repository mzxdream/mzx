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
    MZX_INIT_LIST_HEAD(&entity_list_);
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
    return iter_entity->second;
}

Entity * EntityManager::AddEntity()
{
    MZX_CHECK_STATIC(sizeof(Entity *) == sizeof(EventID));
    Entity *entity = new Entity(*this);
    entity->SetID((EntityID)entity);
    entities_[entity->ID()] = entity;
    MZX_LIST_PUSH_BACK(&entity->list_link_, &entity_list_);
    entity->IncrRef();
    entity_add_event_.Invoke(entity);
    if (entity->invalid_)
    {
        entity->DecrRef();
        return nullptr;
    }
    entity->DecrRef();
    return entity;
}

void EntityManager::RemoveEntity(EntityID id)
{
    auto iter_entity = entities_.find(id);
    if (iter_entity == entities_.end())
    {
        return;
    }
    auto *entity = iter_entity->second;
    entities_.erase(iter_entity);
    entity->IncrRef();
    entity->SelfRemove();
    entity_remove_event_.Invoke(entity);
    entity->DecrRef();
}

void EntityManager::RemoveAllEntity()
{
    for (auto iter = MZX_LIST_BEGIN(&entity_list_); iter != MZX_LIST_END(&entity_list_);)
    {
        auto *entity = MZX_LIST_ENTRY(iter, Entity, list_link_);
        if (entity->invalid_)
        {
            iter = MZX_LIST_NEXT(iter);
            continue;
        }
        entities_.erase(entity->ID());
        entity->IncrRef();
        entity->SelfRemove();
        entity_remove_event_.Invoke(entity);
        iter = MZX_LIST_NEXT(iter);
        entity->DecrRef();
    }
}

void EntityManager::ForeachEntity(std::function<bool (Entity *)> cb)
{
    if (!cb)
    {
        return;
    }
    for (auto iter = MZX_LIST_BEGIN(&entity_list_); iter != MZX_LIST_END(&entity_list_);)
    {
        auto *entity = MZX_LIST_ENTRY(iter, Entity, list_link_);
        if (entity->invalid_)
        {
            iter = MZX_LIST_NEXT(iter);
            continue;
        }
        entity->IncrRef();
        if (!cb(entity))
        {
            entity->DecrRef();
            return;
        }
        iter = MZX_LIST_NEXT(iter);
        entity->DecrRef();
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
    , invalid_(false)
    , ref_count_(1)
{
    MZX_INIT_LIST_HEAD(&list_link_);
}

Entity::~Entity()
{
    MZX_CHECK(invalid_ && ref_count_ == 0);
    RemoveAllComponent();
    MZX_LIST_REMOVE(&list_link_);
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

void Entity::SetID(EntityID id)
{
    id_ = id;
}

void Entity::IncrRef()
{
    ++ref_count_;
}

void Entity::DecrRef()
{
    MZX_CHECK(ref_count_ > 0);
    if (--ref_count_ == 0)
    {
        delete this;
    }
}

void Entity::SelfRemove()
{
    if (!invalid_)
    {
        RemoveAllComponent();
        invalid_ = true;
        DecrRef();
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
    : system_index_(EntitySystemBase::ClassIndexCount())
{
    MZX_INIT_LIST_HEAD(&system_list_);
}

EntitySystemManager::~EntitySystemManager()
{
    RemoveAllSystem();
}

void EntitySystemManager::RemoveAllSystem()
{
    for (auto it = MZX_LIST_BEGIN(&system_list_); it != MZX_LIST_END(&system_list_); it = MZX_LIST_BEGIN(&system_list_))
    {
        auto *node = MZX_LIST_ENTRY(it, SystemNode, list_link);
        node->SelfRemove();
    }
}

void EntitySystemManager::UpdateAll()
{
    auto it = MZX_LIST_BEGIN(&system_list_);
    while (it != MZX_LIST_END(&system_list_))
    {
        auto *node = MZX_LIST_ENTRY(it, SystemNode, list_link);
        node->IncrRef();
        if (node->system)
        {
            node->system->Update();
        }
        it = MZX_LIST_NEXT(it);
        node->DecrRef();
    }
}

}
