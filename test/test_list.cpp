#include <iostream>
#include <mzx/list.h>

struct Test
{
    int a;
    mzx::ListHead list_link;
};

int main(int argc, char *argv[])
{
    mzx::ListHead test_list;
    Test a1{.a=1};
    a1.list_link.PushIntoBack(&test_list);
    Test a2{.a=2};
    a2.list_link.PushIntoFront(&test_list);
    Test a3{.a=3};
    a3.list_link.PushIntoBack(&test_list);

    for (auto *node = test_list.Next(); node != &test_list; node = node->Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, Test, list_link);
        std::cout << entry->a << ",";
    }
    std::cout << std::endl;
    return 0;
}
