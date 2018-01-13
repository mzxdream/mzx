#ifndef __MZX_ECS_ENTITY_MANAGER_H__
#define __MZX_ECS_ENTITY_MANAGER_H__

#include <functional>

#include <mzx/ecs/entity.h>

namespace mzx {
namespace ecs {

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager & operator=(const EntityManager &) = delete;
public:
    Entity * CreateEntity();
    void DestroyEntity(Entity::IDType id);
    void DestroyAllEntity();
    Entity * GetEntity(Entity::IDType id);
    template <typename T, typename ...Args>
    void ForeachEntity(const std::function<void (Entity *)> &cb)
    {
        for (auto &iter_entity : entity_list_)
        {
            if (!iter_entity.second->HasComponent<T, Args...>())
            {
                continue;
            }
            cb(iter_entity.second);
        }
    }
    void ForeachAllEntity(const std::function<void (Entity *)> &cb);
private:
    Entity::IDType next_entity_id_;
    std::map<Entity::IDType, Entity *> entity_list_;
};

}
}

#endif
