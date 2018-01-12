#include <mzx/ecs/entity.h>

namespace mzx {
namespace ecs {

Entity::Entity(Entity::IDType id)
    : id_(id)
{
}

Entity::~Entity()
{
    RemoveAllComponent();
}

Entity::IDType Entity::ID() const
{
    return id_;
}

}
}
