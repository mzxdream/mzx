#include <mzx/noncopyable.h>
#include <iostream>

struct Test
    : mzx::Noncopyable
{
    int a{ 0 };
};

int main(int argc, char *argv[])
{
    Test a;
    //Test b = a;
    Test b;
    //b = a;
    return 0;
}
