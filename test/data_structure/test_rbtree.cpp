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

void PrintNode(mzx::RBTreeNode *node)
{
    if (!node)
    {
        return;
    }
    std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << ",";
    PrintNode(node->Left());
    PrintNode(node->Right());
}

void PrintTree(const mzx::RBTree<int, TestKeyOfNode> &rbtree)
{
    std::cout << std::endl;
    PrintNode(rbtree.Root());
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    std::srand(time(0));
    mzx::RBTree<int, TestKeyOfNode> rbtree;
    mzx::RBTree<int, TestKeyOfNode> rbtree2;
    std::vector<Test> ttt;
    int len = std::rand() % 6 + 5;
    for (int i = 0; i < len; ++i)
    {
        ttt.emplace_back(std::rand() % 20);
    }
    for (auto &t : ttt)
    {
        auto ret = rbtree.Insert(&t.rbtree_link);
        std::cout << t.a << "," << ret << ";";
    }
    std::vector<Test> ttt2 = ttt;
    for (auto &t : ttt2)
    {
        rbtree2.InsertEqual(&t.rbtree_link);
    }
    std::cout << std::endl;
    std::cout << "====:";
    for (auto *node = rbtree.First(); node != nullptr; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << ",";
    }
    std::cout << std::endl;
    std::cout << "====:";
    for (auto *node = rbtree2.First(); node != nullptr; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << ",";
    }
    std::cout << std::endl;
    std::cout << "====:" << std::endl;
    for (auto *node = rbtree.First(); node != nullptr;)
    {
        auto *tmp = node->Next();
        std::cout << "remove:" << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << " cur len:" << rbtree.Length()
                  << std::endl;
        rbtree.Erase(node);
        node = tmp;
    }
    std::cout << "====:";
    for (auto *node = rbtree.First(); node != nullptr; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << ",";
    }
    std::cout << std::endl;
    return 0;
}
