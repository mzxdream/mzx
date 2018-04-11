#ifndef __MZX_ENTITY_SYSTEM_H__
#define __MZX_ENTITY_SYSTEM_H__

#include <list>
#include <cstdint>

namespace mzx {

class EntitySystemBase
{
public:
    using ClassIndexType = std::size_t;
public:
    virtual ~EntitySystemBase() = 0;
    EntitySystemBase(const EntitySystemBase &) = delete;
    EntitySystemBase & operator=(const EntitySystemBase &) = delete;
public:
    virtual bool Init() { return true; }
    virtual void Uninit() {}
    virtual void Update(int64_t time_delta) {}
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class EntitySystem
    : public EntitySystemBase
{
public:
    const static ClassIndexType CLASS_INDEX = ++EntitySystemBase::next_class_index_;
};

class EntitySystemManager
{
public:
    EntitySystemManager();
    ~EntitySystemManager();
    EntitySystemManager(const EntitySystemManager &) = delete;
    EntitySystemManager & operator=(const EntitySystemManager &) = delete;
public:
    template <typename T, typename ...Args>
    T * AddSystem(Args && ...args)
    {
        static_assert(std::is_base_of<EntitySystem<T>, T>::value, "system must be extern EntitySystem<T>");
        T *system = new T(std::forward<Args>(args)...);
        system_list_.push_back(system);
        return system;
    }
    bool InitAllSystem();
    void UninitAllSystem();
    void UpdateAllSystem(int64_t time_delta);
    void ClearAllSystem();
private:
    std::list<EntitySystemBase *> system_list_;
};

}

#endif
