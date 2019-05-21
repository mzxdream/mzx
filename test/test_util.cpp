#include <functional>
#include <iostream>

#include <mzx/util.h>

int main(int argc, char *argv[])
{
    int a = 0;
    std::function<void(int, int)> b;
    std::cout << "a:" << typeid(a).name() << "->" << mzx::Demangle(typeid(a).name()) << std::endl;
    std::cout << "b:" << typeid(b).name() << "->" << mzx::Demangle(typeid(b).name()) << std::endl;
    return 0;
}
