#ifndef __MZX_RBTREE_H__
#define __MZX_RBTREE_H__

#include <functional>

#include <mzx/logger.h>

namespace mzx
{

template <typename Key, typename KeyOfNode, typename Compare>
class RBTree;

class RBTreeNode final
{
    template <typename Key, typename KeyOfNode, typename Compare>
    friend class RBTree;

public:
    RBTreeNode();
    ~RBTreeNode();

    inline bool IsLinked() const
    {
        return parent_color_ == reinterpret_cast<unsigned long>(this);
    }
    RBTreeNode *Prev();
    RBTreeNode *Next();

private:
    inline unsigned long ParentColor() const
    {
        return parent_color_;
    }
    inline RBTreeNode *Parent() const
    {
        return reinterpret_cast<RBTreeNode *>(parent_color_ & ~3);
    }
    inline bool IsBlack() const
    {
        return parent_color_ & 1;
    }
    inline bool IsRed() const
    {
        return !IsBlack();
    }
    inline RBTreeNode *Left() const
    {
        return left_;
    }
    inline RBTreeNode *Right() const
    {
        return right_;
    }

private:
    inline void Clear()
    {
        parent_color_ = reinterpret_cast<unsigned long>(this);
        left_ = nullptr;
        right_ = nullptr;
    }
    inline void SetParentColor(unsigned long parent_color)
    {
        parent_color_ = parent_color;
    }
    inline void SetParentColor(RBTreeNode *parent, bool is_black)
    {
        parent_color_ = reinterpret_cast<unsigned long>(parent) | is_black;
    }
    inline void SetParent(RBTreeNode *parent)
    {
        parent_color_ = reinterpret_cast<unsigned long>(parent) | (parent_color_ & 1);
    }
    inline void SetBlack()
    {
        parent_color_ |= 1;
    }
    inline void SetRed()
    {
        parent_color_ &= ~3;
    }
    inline void SetLeft(RBTreeNode *node)
    {
        left_ = node;
    }
    inline void SetRight(RBTreeNode *node)
    {
        right_ = node;
    }
    void Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root);
    void Erase(RBTreeNode **root);

private:
    static RBTreeNode *Parent(unsigned parent_color)
    {
        return reinterpret_cast<RBTreeNode *>(parent_color & ~3);
    }
    static bool IsBlack(unsigned long parent_color)
    {
        return parent_color & 1;
    }
    static bool IsRed(unsigned long parent_color)
    {
        return !RBTreeNode::IsBlack(parent_color);
    }

private:
    static void _ChangeChild(RBTreeNode *old_node, RBTreeNode *new_node, RBTreeNode *parent, RBTreeNode **root);
    static void _RotateSetParents(RBTreeNode *old_node, RBTreeNode *new_node, RBTreeNode **root, bool is_black);
    static void _Insert(RBTreeNode *node, RBTreeNode **root);
    static void _Erase(RBTreeNode *node, RBTreeNode **root);

private:
    unsigned long parent_color_{0};
    RBTreeNode *left_{nullptr};
    RBTreeNode *right_{nullptr};
};

template <typename Key, typename KeyOfNode, typename Compare = std::less<Key>>
class RBTree final
{
public:
    RBTree()
    {
    }
    ~RBTree()
    {
    }

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
                link = &parent->left_;
            }
            else
            {
                link = &parent->right_;
            }
        }
        // Check euqal
        node->Insert(parent, link, &root_);
        return true;
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
                link = &parent->left_;
            }
            else
            {
                link = &parent->right_;
            }
        }
        node->Insert(parent, link, &root_);
    }
    void Erase(RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->Erase(&root_);
    }
    RBTreeNode *First() const
    {
        if (!root_)
        {
            return nullptr;
        }
        RBTreeNode *node = root_;
        while (node->Left())
        {
            node = node->Left();
        }
        return node;
    }
    RBTreeNode *Last() const
    {
        if (!root_)
        {
            return nullptr;
        }
        RBTreeNode *node = root_;
        while (node->Right())
        {
            node = node->Right();
        }
        return node;
    }

private:
    RBTreeNode *root_{nullptr};
    KeyOfNode key_of_node_;
    Compare key_comp_;
};

} // namespace mzx

#endif
