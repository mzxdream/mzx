#ifndef __MZX_ECS_SYSTEM_H__
#define __MZX_ECS_SYSTEM_H__

#include <cstdint>

namespace mzx {
namespace ecs {

class EntityManager;
class EventManager;
class SystemManager;

class SystemBase
{
    friend class SystemManager;
public:
    typedef std::size_t ClassIndexType;
private:
    virtual ~SystemBase() = 0;
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
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class System
    : public SystemBase
{
    friend class SystemManager;
public:
    const static ClassIndexType CLASS_INDEX = ++SystemBase::next_class_index_;
public:
    virtual ~System()
    {
    }
};

}
}

#endif
