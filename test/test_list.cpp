#include <iostream>
#include <mzx/list.h>

struct Test
{
    int a;
    mzx::ListHead list;
};

int main(int argc, char *argv[])
{
    mzx::ListHead test_list;
    MZX_INIT_LIST_HEAD(&test_list);
    Test a1{.a=1};
    MZX_LIST_PUSH_BACK(&a1.list, &test_list);
    Test a2{.a=2};
    MZX_LIST_PUSH_FRONT(&a2.list, &test_list);
    Test a3{.a=3};
    MZX_LIST_PUSH_BACK(&a3.list, &test_list);
    MZX_LIST_FOREACH_SAFE(iter, &test_list)
    {
        MZX_LIST_REMOVE(&a2.list);
        auto *entry = MZX_LIST_ENTRY(iter, Test, list);
        std::cout << entry->a << ",";
    }
    std::cout << std::endl;
    MZX_LIST_FOREACH_REVERSE_SAFE(iter, &test_list)
    {
        auto *entry = MZX_LIST_ENTRY(iter, Test, list);
        std::cout << entry->a << ",";
    }
    std::cout << std::endl;
    return 0;
}
