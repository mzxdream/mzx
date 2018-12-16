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

    return 0;
}
