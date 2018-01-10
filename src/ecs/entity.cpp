#include <mzx/ecs/entity.h>

namespace mzx {
namespace ecs {

Entity::Entity(Entity::IDType id)
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

Entity::IDType Entity::ID() const
{
    return id_;
}

}
}
