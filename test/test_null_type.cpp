#include <mzx/null_type.h>
#include <iostream>

template <typename T1 = mzx::NullType<>, typename T2 = mzx::NullType<>, typename T3 = mzx::NullType<> >
struct Test
{
    Test(T1 a, T2 b, T3 c)
        : a1(a)
        , a2(b)
        , a3(c)
    {
    }
    T1 a1;
    T2 a2;
    T3 a3;
};

template <typename T1, typename T2>
struct Test<T1, T2, mzx::NullType<> >
{
    T1 a1;
    T2 a2;
};

template <typename T1>
struct Test<T1, mzx::NullType<>, mzx::NullType<> >
{
    Test(T1 a)
        : a1(a)
    {
    }
    T1 a1;
};

int main(int argc, char *argv[])
{
    Test<int, double, std::string> a(1, 1.0, "123");
    std::cout << a.a1 << "," << a.a2 << "," << a.a3 << std::endl;
    Test<int, double> b;
    b.a1 = 1;
    b.a2 = 1.0;
    //b.a3 = "123";
    std::cout << b.a1 << "," << b.a2 << std::endl;

    Test<int> c(15);
    std::cout << c.a1 << std::endl;
    return 0;
}
