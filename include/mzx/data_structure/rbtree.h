#ifndef __MZX_RBTREE_H__
#define __MZX_RBTREE_H__

#include <functional>

#include <mzx/logger.h>

namespace mzx
{

class RBTreeNode final
{
public:
    enum class Color
    {
        Red = 0,
        Black = 1
    };

public:
    RBTreeNode *Parent();
    RBTreeNode *Left();
    RBTreeNode *Right();

    void Link(RBTreeNode *parent, RBTreeNode **link)
    {
        parent_color_ = static_cast<unsigned long>(parent);
        left_ = nullptr;
        right_ = nullptr;
        *link = this;
    }
    void SetParentColor(RBTreeNode *parent, Color color);

private:
    unsigned long parent_color_{0};
    RBTreeNode *left_{nullptr};
    RBTreeNode *right_{nullptr};
};

template <typename Key, typename KeyOfNode, typename Compare = std::less<Key>>
class RBTree final
{
public:
    RBTree();
    ~RBTree();

public:
    RBTreeNode *Find(Key key)
    {
        RBTreeNode *found = nullptr;
        auto *node = root_;
        while (node)
        {
            if (!key_comp_(key_of_node_(node), key))
            {
                found = node; // because key equal
                node = node->left_;
            }
            else
            {
                node = node->right_;
            }
        }
        if (!found)
        {
            return nullptr;
        }
        return found && !key_comp_(key, key_of_node_(node)) ? found : nullptr;
    }

    bool Insert(RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        auto **link = &root_;
        RBTreeNode *parent = nullptr;
        while (*link)
        {
            parent = *link;
            if (key_comp_(key_of_node_(node), key_of_node_(parent)))
            {
                link = &parent->Left();
            }
            else
            {
                link = &parent->Right();
            }
        }
        node->Link(parent, link);
        //check repeat
        // TODO
    }
    void InsertEqual(RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        auto **link = &root_;
        RBTreeNode *parent = nullptr;
        while (*link)
        {
            parent = *link;
            if (key_comp_(key_of_node_(node), key_of_node_(parent)))
            {
                link = &parent->Left();
            }
            else
            {
                link = &parent->Right();
            }
        }
        node->Link(parent, link);
        // TODO
    }

private:
    RBTreeNode *root_{nullptr};
    KeyOfNode key_of_node_;
    Compare key_comp_;
};

} // namespace mzx
