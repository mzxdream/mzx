#include <iostream>
#include <mzx/event.h>

void PrintNum(int i)
{
    std::cout << i << ",";
}

int main(int argc, char *argv[])
{
    mzx::Event<void ()> event;
    event.AddListener(std::bind(PrintNum, 0));
    event.AddListener(std::bind(PrintNum, 1));
    event.AddListener(std::bind(PrintNum, 2));
    event.AddListener([&](){
        event.RemoveAllListener();
        event.RemoveAllListener();
    });
    event.AddListener(std::bind(PrintNum, 3));
    event.Invoke();
    std::cout << std::endl;

    mzx::EventManager<int, void ()> event_manager;
    event_manager.AddListener(0, std::bind(PrintNum, 0));
    event_manager.AddListener(0, std::bind(PrintNum, 1));
    event_manager.AddListener(1, std::bind(PrintNum, 2));
    event_manager.AddListener(0, [&](){
        event_manager.RemoveEvent(0);
    });
    event_manager.AddListener(0, std::bind(PrintNum, 3));
    event_manager.Invoke(0);
    event_manager.Invoke(1);
    std::cout << std::endl;

    return 0;
}
