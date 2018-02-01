#include <mzx/ecs/entity.h>

namespace mzx {
namespace ecs {

Entity::Entity(Entity::ID id)
    : id_(id)
{
}

Entity::~Entity()
{
    RemoveAllComponent();
}

Entity::ID Entity::Id() const
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

}
}
