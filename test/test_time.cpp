#include <mzx/time.h>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    mzx::Time::Sleep(1000);
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    mzx::Time::Sleep(2000);
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    return 0;
}
