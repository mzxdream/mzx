#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>

#include <mzx/data_structure/rbtree.h>
#include <mzx/macro_util.h>

struct Test
{
    explicit Test(int i)
        : a(i)
    {
    }
    int a{0};
    mzx::RBTreeNode rbtree_link;
};

struct TestKeyOfNode
{
    int operator()(mzx::RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        return MZX_CONTAINER_OF(node, Test, rbtree_link)->a;
    }
};

int main(int argc, char *argv[])
{
    std::srand(time(0));
    mzx::RBTree<int, TestKeyOfNode> rbtree;
    std::vector<Test> ttt;
    int len = std::rand() % 6 + 5;
    for (int i = 0; i < len; ++i)
    {
        ttt.emplace_back(std::rand() % 20);
    }
    for (auto &t : ttt)
    {
        rbtree.Insert(&t.rbtree_link);
        std::cout << t.a << ",";
    }
    std::cout << std::endl;
    for (auto *node = rbtree.First(); node != nullptr; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << "," << std::endl;
    }
    return 0;
}
