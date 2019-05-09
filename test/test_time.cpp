#include <iostream>
#include <mzx/time.h>

int main(int argc, char *argv[])
{
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    mzx::Time::Sleep(1000);
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    mzx::Time::Sleep(2000);
    std::cout << "=====:" << mzx::Time::Now() << std::endl;
    return 0;
}
