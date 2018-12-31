#include <mzx/ecs.h>
#include <iostream>

struct TestComponent
{
    explicit TestComponent(int a)
        : a(a)
    {
        std::cout << "TestComponent construct" << std::endl;
    }
    ~TestComponent()
    {
        std::cout << "TestComponent destruct" << std::endl;
    }
    int a{ 0 };
};

struct TestComponent2
{
    TestComponent2()
    {
        std::cout << "TestComponent2 construct" << std::endl;
    }
    ~TestComponent2()
    {
        std::cout << "TestComponent2 destruct" << std::endl;
    }
    int a{ 0 };
};

class World
{
public:
    World()
    {

    }
    ~World()
    {

    }
public:
    mzx::EntityManager & GetEntityManager() { return entity_manager_; }
    mzx::EntitySystemManager & GetSystemManager() { return system_manager_; }
    mzx::EventManager<int, void()> & GetEventManager() { return event_manager_; }
private:
    mzx::EntityManager entity_manager_;
    mzx::EntitySystemManager system_manager_;
    mzx::EventManager<int, void()> event_manager_;
};

class TestSystem
    : public mzx::EntitySystem<TestSystem>
{
public:
    TestSystem(World &world)
        : world_(world)
    {
    }
private:
    virtual bool _Init() override
    {
        world_.GetEventManager().AddListener(1, [this](){
            std::cout << "this is test system id:" << ClassIndex() << std::endl;
        });
        world_.GetEntityManager().ComponentAddEvent().AddListener([](mzx::Entity *entity, mzx::ComponentBase *component){
            std::cout << "entity:" << entity->ID() << " add component" << component->ClassIndex() << std::endl;
        });
        world_.GetEntityManager().ComponentRemoveEvent().AddListener([](mzx::Entity *entity, mzx::ComponentBase *component){
            std::cout << "entity:" << entity->ID() << " remove component" << component->ClassIndex() << std::endl;
        });
        return true;
    }
    virtual void _Uninit() override
    {

    }
    virtual void _Update() override
    {
        std::cout << "test begin" << std::endl;
        world_.GetEntityManager().ForeachEntity([](mzx::Entity *entity){
            if (entity->HasComponent<TestComponent>())
            {
                std::cout << "entity:" << entity->ID()
                    << " test component:" << entity->GetComponent<TestComponent>()->a << std::endl;
            }
            return true;
        });
        std::cout << "test end" << std::endl;
    }
private:
    World &world_;
};

class TestSystem2
    : public mzx::EntitySystem<TestSystem2>
{
public:
    TestSystem2(World &world)
        : world_(world)
    {
    }
private:
    virtual bool _Init() override
    {
        world_.GetEventManager().AddListener(1, [this](){
            std::cout << "this is test system2 id:" << ClassIndex() << std::endl;
        });
        return true;
    }
    virtual void _Uninit() override
    {

    }
    virtual void _Update() override
    {
        std::cout << "test2 begin" << std::endl;
        world_.GetEntityManager().ForeachEntity([](mzx::Entity *entity){
            if (entity->HasComponent<TestComponent2>())
            {
                std::cout << "entity:" << entity->ID()
                    << " test component2:" << entity->GetComponent<TestComponent2>()->a << std::endl;
            }
            return true;
        });
        std::cout << "test2 end" << std::endl;
    }
private:
    World &world_;
};

int main(int argc, char *argv[])
{
    World world;
    world.GetSystemManager().AddSystem<TestSystem>(world);
    world.GetSystemManager().AddSystem<TestSystem2>(world);
    auto *e1 = world.GetEntityManager().AddEntity();
    e1->AddComponent<TestComponent>(123);
    auto *e2 = world.GetEntityManager().AddEntity();
    auto *c2 = e2->AddComponent<TestComponent2>();
    c2->a = 234;
    auto *e3 = world.GetEntityManager().AddEntity();
    e3->AddComponent<TestComponent>(456);
    e3->AddComponent<TestComponent2>();
    e3->GetComponent<TestComponent2>()->a = 789;
    world.GetSystemManager().UpdateAll();
    return 0;
}
