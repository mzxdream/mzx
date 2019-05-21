#include <functional>
#include <iostream>

#include <mzx/system/sys_util.h>

int main(int argc, char *argv[])
{
    int a = 0;
    std::function<void(int, int)> b;
    std::cout << "a: " << typeid(a).name() << " -> " << mzx::Demangle(typeid(a).name()) << std::endl;
    std::cout << "b: " << typeid(b).name() << " -> " << mzx::Demangle(typeid(b).name()) << std::endl;
    std::cout << "CPU count:" << mzx::GetCPUCount() << std::endl;
    mzx::ForeachStackTrace([](const char *symbol) {
        std::cout << "symbol:" << symbol << std::endl;
        return true;
    });
    return 0;
}
