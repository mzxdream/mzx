#include <functional>
#include <iostream>

#include <mzx/system/sys_util.h>

void Test3()
{
    std::cout << 2 << std::endl;
    mzx::ForeachStackTrace([](const char *symbol) {
        std::cout << "symbol:" << symbol << std::endl;
        return true;
    });
}

void Test2()
{
    std::cout << 2 << std::endl;
    Test3();
}

void Test1()
{
    std::cout << 1 << std::endl;
    Test2();
}

void Test()
{
    std::cout << 0 << std::endl;
    Test1();
}

int main(int argc, char *argv[])
{
    int a = 0;
    std::function<void(int, int)> b;
    std::cout << "a: " << typeid(a).name() << " -> " << mzx::Demangle(typeid(a).name()) << std::endl;
    std::cout << "b: " << typeid(b).name() << " -> " << mzx::Demangle(typeid(b).name()) << std::endl;
    std::cout << "CPU count:" << mzx::GetCPUCount() << std::endl;
    Test();
    return 0;
}
