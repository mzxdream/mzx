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
    int operator()(const mzx::RBTreeNode *node) const
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
    std::vector<Test> ttt3{Test(0), Test(1), Test(2), Test(2), Test(3), Test(4), Test(6), Test(4), Test(4)};
    mzx::RBTree<int, TestKeyOfNode> rbtree3;
    for (auto &t : ttt3)
    {
        rbtree3.InsertEqual(&t.rbtree_link);
    }
    std::cout << "====:";
    for (auto *node = rbtree3.First(); node != nullptr; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << ",";
    }
    std::cout << std::endl;
    int tmp = std::rand() % 10;
    std::cout << "tmp:" << tmp << std::endl;
    auto *found = rbtree3.Find(tmp);
    auto *lower = rbtree3.LowerBound(tmp);
    auto *upper = rbtree3.UpperBound(tmp);
    std::cout << "1:" << found << " 2:" << lower << " 3:" << upper << std::endl;
    for (auto *node = lower; node != upper; node = node->Next())
    {
        std::cout << MZX_CONTAINER_OF(node, Test, rbtree_link)->a << std::endl;
    }
    return 0;
}
