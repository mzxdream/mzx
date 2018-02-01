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

}
}
