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

    enum class Color : bool
    {
        Red = 0,
        Black = 1,
    };

    using ParentColorType = unsigned long;

public:
    RBTreeNode();
    ~RBTreeNode();

    bool IsLinked() const
    {
        return parent_color_ != reinterpret_cast<ParentColorType>(this);
    }
    RBTreeNode *Prev();
    RBTreeNode *Next();

public:
    ParentColorType ParentColor() const
    {
        return parent_color_;
    }
    RBTreeNode *Parent() const
    {
        return RBTreeNode::Parent(parent_color_);
    }
    bool IsBlack() const
    {
        return parent_color_ & 1;
    }
    bool IsRed() const
    {
        return !IsBlack();
    }
    RBTreeNode *Left() const
    {
        return left_;
    }
    RBTreeNode *Right() const
    {
        return right_;
    }

private:
    void Clear()
    {
        parent_color_ = reinterpret_cast<ParentColorType>(this);
        left_ = nullptr;
        right_ = nullptr;
    }
    void SetParentColor(ParentColorType parent_color)
    {
        parent_color_ = parent_color;
    }
    void SetParentColor(RBTreeNode *parent, Color color)
    {
        parent_color_ = reinterpret_cast<ParentColorType>(parent) | static_cast<ParentColorType>(color);
    }
    void SetParent(RBTreeNode *parent)
    {
        parent_color_ = reinterpret_cast<ParentColorType>(parent) | (parent_color_ & 1);
    }
    void SetBlack()
    {
        parent_color_ |= 1;
    }
    void SetRed()
    {
        parent_color_ &= ~3;
    }
    void SetLeft(RBTreeNode *node)
    {
        left_ = node;
    }
    void SetRight(RBTreeNode *node)
    {
        right_ = node;
    }
    void ChangeChild(RBTreeNode *new_node, RBTreeNode *parent, RBTreeNode **root);
    void RotateSetParents(RBTreeNode *new_node, RBTreeNode **root, Color color);
    void Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root);
    void Erase(RBTreeNode **root);

private:
    static RBTreeNode *Parent(ParentColorType parent_color)
    {
        return reinterpret_cast<RBTreeNode *>(parent_color & ~3);
    }
    static bool IsBlack(ParentColorType parent_color)
    {
        return parent_color & 1;
    }
    static bool IsRed(ParentColorType parent_color)
    {
        return !RBTreeNode::IsBlack(parent_color);
    }

private:
    ParentColorType parent_color_{0};
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
    RBTreeNode *Root() const
    {
        return root_;
    }
    std::size_t Length() const
    {
        return length_;
    }
    bool Empty() const
    {
        return length_ == 0;
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
        RBTreeNode *check_node = nullptr;
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
                check_node = parent;
            }
        }
        if (check_node && !key_comp_(key_of_node_(check_node), key_of_node_(node)))
        {
            return false;
        }
        node->Insert(parent, link, &root_);
        ++length_;
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
        ++length_;
    }
    RBTreeNode *Erase(Key key)
    {
        auto *node = Find(key);
        if (!node)
        {
            return nullptr;
        }
        Erase(node);
        return node;
    }
    void Erase(RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->Erase(&root_);
        --length_;
    }

private:
    RBTreeNode *root_{nullptr};
    KeyOfNode key_of_node_;
    Compare key_comp_;
    std::size_t length_{0};
};

} // namespace mzx

#endif
