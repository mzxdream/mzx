#include <mzx/ecs/entity.h>

namespace mzx {
namespace ecs {

Entity::Entity(Entity::ID id)
    : id_(id)
{
}

Entity::~Entity()
{
    for (auto iter_component : component_list_)
    {
        delete iter_component.second;
    }
    component_list_.clear();
}

Entity::ID Entity::GetID() const
{
    return id_;
}

}
}
