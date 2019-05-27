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
    mzx::RBTree<int, TestKeyOfNode> rbtree;
    std::vector<Test> ttt;
    for (int i = 0; i < 20; i++)
    {
        ttt.push_back(Test(i));
    }
    for (auto &t : ttt)
    {
        rbtree.Insert(&t.rbtree_link);
    }
    return 0;
}
