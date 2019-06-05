#include <iostream>

#include <mzx/thread.h>

int main(int argc, char *argv[])
{
    mzx::Thread th1([]() {
        while (1)
        {
            std::cout << "--------------" << std::endl;
            mzx::Thread::Sleep(1000);
            std::cout << "==============" << std::endl;
            mzx::Thread::CheckCancelPoint();
            std::cout << "++++++++++++++" << std::endl;
        }
    });
    std::cout << "0000:" << th1.ID() << " , " << th1.Joinable() << std::endl;
    th1.Start();
    std::cout << "1111:" << th1.ID() << " , " << th1.Joinable() << std::endl;
    mzx::Thread::Sleep(10000);
    std::cout << "2222:" << th1.ID() << " , " << th1.Joinable() << std::endl;
    th1.Cancel();
    std::cout << "3333:" << th1.ID() << " , " << th1.Joinable() << std::endl;
    th1.Join();
    std::cout << "4444:" << th1.ID() << " , " << th1.Joinable() << std::endl;
    return 0;
}
