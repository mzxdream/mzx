#include <iostream>
#include <mzx/list.h>

struct Test
{
    int a;
    mzx::ListNode list_link;
};

int main(int argc, char *argv[])
{
    mzx::ListNode test_list;
    Test a1{.a=1};
    test_list.PushBack(&a1.list_link);
    Test a2{.a=2};
    test_list.PushBack(&a2.list_link);
    Test a3{.a=3};
    test_list.PushFront(&a3.list_link);
    for (auto *node = test_list.Next(); node != &test_list; node = node->Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, Test, list_link);
        std::cout << entry->a << ",";
        ++entry->a;
    }
    std::cout << std::endl;
    for (auto *node = test_list.Next(); node != &test_list; node = node->Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, Test, list_link);
        std::cout << entry->a << ",";
    }
    std::cout << std::endl;
    for (auto *node = test_list.Next(); node != &test_list; )
    {
        auto *entry = MZX_LIST_ENTRY(node, Test, list_link);
        std::cout << entry->a << ",";
        if (entry->a >= 3)
        {
            node = node->Unlink();
        }
        else
        {
            node = node->Next();
        }
    }
    std::cout << std::endl;
    std::cout << "1111" << std::endl;
    for (auto *node = test_list.Next(); node != &test_list; node = node->Next())
    {
        auto *entry = MZX_LIST_ENTRY(node, Test, list_link);
        std::cout << entry->a << ",";
    }
    std::cout << "2222" << std::endl;
    return 0;
}
