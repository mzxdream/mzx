#include <iostream>
#include <mzx/null_mutex.h>
#include <mutex>

int main(int argc, char *argv[])
{
    mzx::NullMutex null_mutex;
    std::mutex mtx;
    std::lock_guard<mzx::NullMutex> lock1(null_mutex);
    std::lock_guard<std::mutex> lock2(mtx);
    return 0;
}
