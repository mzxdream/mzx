#include <mzx/default_type.h>
#include <iostream>
#include <map>

struct Test1
{
    Test1(int t = 1234)
        : a(t)
    {
    }
    int a;
};

class Test
{
public:
    const Test1 & GetValue(int a) const
    {
        auto iter = test_list.find(a);
        if (iter == test_list.end())
        {
            return mzx::DefaultType<Test1>::Value;
        }
        return iter->second;
    }
    std::map<int, Test1> test_list;
};

int main(int argc, char *argv[])
{
    Test t;
    t.test_list[1] = Test1(1);
    t.test_list[3] = Test1(3);
    for (int i = 1; i <= 3; ++i)
    {
        std::cout << t.GetValue(i).a << ",";
    }
    std::cout << std::endl;
    return 0;
}
