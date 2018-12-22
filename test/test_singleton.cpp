#include <mzx/singleton.h>
#include <iostream>

class Test1
    : public mzx::Singleton<Test1>
{
    friend mzx::Singleton<Test1>;
private:
    Test1()
        : a(10)
    {

    }
public:
    int a;
};

class Test2
{
public:
    int a{ 5 };
};

int main(int argc, char *argv[])
{
    std::cout << Test1::Instance().a << std::endl;
    mzx::Singleton<Test2>::Instance().a = 100;
    Test2 t2;
    t2.a = 10;
    std::cout << mzx::Singleton<Test2>::Instance().a << std::endl;
    std::cout << mzx::Singleton<Test2, std::mutex>::Instance().a << std::endl;
    std::cout << t2.a << std::endl;
    return 0;
}
