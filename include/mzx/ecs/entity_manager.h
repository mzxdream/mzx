#ifndef __MZX_ENTITY_MANAGER_H__
#define __MZX_ENTITY_MANAGER_H__

#include <functional>

#include <mzx/ecs/entity.h>

namespace mzx {

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager &) = delete;
    EntityManager & operator=(const EntityManager &) = delete;
public:
    Entity * CreateEntity();
    void DestroyEntity(Entity::ID id);
    void DestroyAllEntity();
    Entity * GetEntity(Entity::ID id);
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
    void ForeachEntity(const std::function<void (Entity *)> &cb);
private:
    Entity::ID next_entity_id_;
    std::map<Entity::ID, Entity *> entity_list_;
};

}

#endif
