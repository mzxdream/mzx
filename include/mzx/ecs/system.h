#ifndef __MZX_ECS_SYSTEM_H__
#define __MZX_ECS_SYSTEM_H__

#include <cstdint>

namespace mzx {
namespace ecs {

class EntityManager;
class EventManager;

class System
{
public:
    virtual ~System() = 0;
public:
    virtual void Configure(EntityManager &entity_manager, EventManager &event_manager)
    {
    }
    virtual void Unconfigure(EntityManager &entity_manager, EventManager &event_manager)
    {
    }
    virtual void Update(EntityManager &entity_manager, EventManager &event_manager, int64_t time_delta)
    {
    }
};

}
}

#endif
