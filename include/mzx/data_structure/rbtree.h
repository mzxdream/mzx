#ifndef __MZX_RBTREE_H__
#define __MZX_RBTREE_H__

#include <functional>
#include <mzx/logger.h>

namespace mzx
{

class RBTreeNode final
{
public:
    RBTreeNode(RBTreeNode *parent)
    {
    }
    ~RBTreeNode()
    {
    }

private:
    RBTreeNode *parent_{nullptr};
    RBTreeNode *left_{nullptr};
    RBTreeNode *right_{nullptr};
    bool is_black_{false};
};

template <typename T, typename Compare = std::less<T>>
class Test<T(RBTreeNode *)>
{

    std::function<T(RBTreeNode *)> key_;
};

class RBTree final
{
    friend RBTreeNode;

public:
    using Compare = std::function<bool(const RBTreeNode *, const RBTreeNode *)>;

public:
    RBTree(Compare comp)
        : compare_(comp)
    {
        MZX_CHECK(compare_ != nullptr);
    }
    ~RBTree()
    {
    }

public:
    bool Empty() const;
    bool Insert(RBTreeNode *node)
    {
        MZX_CHECK(node != nullptr);
        auto **tmp = &root_;
        auto *parent = nullptr;
        while (*tmp)
        {
            parent = *tmp;
            if (compare_(node, *tmp))
            {
                tmp = &((*tmp)->left_);
            }
            else if (compare_(*tmp, node))
            {
                tmp = &((*tmp)->right_);
            }
            else
            {
                return false;
            }
        }
        LinkNode(node, parent, tmp);
        InsertColor(node);
    }
    RBTreeNode *Find()
    {
    }

private:
    void LinkNode(RBTreeNode *node, RBTreeNode *parent, RBTreeNode **link)
    {
        node->parent_ = parent;
        node->left_ = nullptr;
        node->right_ = nullptr;
        node->is_black_ = false;
        *link = node;
    }
    void RotateLeft(RBTreeNode *node)
    {
        RBTreeNode *right = node->right_;
        if ((node->right_ = right->left_))
        {
            node->right_->parent = node;
        }
        right->left_ = node;
        if ((right->parent_ = node->parent))
        {
            if (node == node->parent_->left_)
            {
                node->parent_->left_ = right;
            }
            else
            {
                node->parent_->right_ = right;
            }
        }
        else
        {
            root_ = right;
        }
        node->parent_ = right;
    }
    void InsertColor(RBTreeNode *node)
    {
        RBTreeNode *parent = nullptr;
        RBTreeNode *gparent = nullptr;
        RBTreeNode *tmp = nullptr;
        while ((parent = node->parent_) && !parent->is_black_)
        {
            gparent = parent->parent_;
            tmp = gparent->right_;
            if (parent != tmp)
            {
                if (tmp && !tmp->is_black_)
                {
                    tmp->is_black_ = true;
                    parent->is_black_ = true;
                    gparent->is_black_ = false;
                    node = gparent;
                    continue;
                }
                if (parent->right_ == node)
                {
                    RotateLeft(parent);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->is_black_ = true;
                gparent->is_black_ = false;
                RotateRight(gparent);
            }
            else
            {
                tmp = gparent->left_;
                if (tmp && !tmp->is_black_)
                {
                    tmp->is_black_ = true;
                    parent->is_black_ = true;
                    gparent->is_black_ = false;
                    node = gparent;
                    continue;
                }
                if (parent->left_ == node)
                {
                    RotateRight(parent);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->is_black_ = true;
                gparent->is_black_ = false;
                RotateLeft(gparent);
            }
        }
        root_->is_black_ = true;
    }
    void Erase(RBTreeNode *node)
    {
        RBTreeNode *child = nullptr;
        RBTreeNode *parent = nullptr;
        bool is_black_ = false;
        if (!node->left_)
        {
            child = node->right_;
        }
        else if (!node->right_)
        {
            child = node->left_;
        }
        else
        {
            RBTreeNode *old = node;
            RBTreeNode *left = nullptr;
            node = node->right_;
            while ((left = node->left_))
            {
                node = left;
            }
            child = node->right_;
            parent = node->parent_;
            is_black_ = node->is_black_;
            if (child)
            {
                child->parent_ = parent;
            }
            if (parent)
            {
                if (parent->left_ == node)
                {
                    parent->left_ = child;
                }
                else
                {
                    parent->right_ = child;
                }
            }
            else
            {
                root_ = child;
            }
            if (node->parent_ == old)
            {
                parent = node;
            }
            node->parent_ = old->parent_;
            node->is_black_ = old->is_black_;
            node->right_ = old->right_;
            node->left_ = old->left_;
            if (old->parent_)
            {
                if (old->parent_->left_ == old)
                {
                    old->parent_->left_ = node;
                }
                else
                {
                    old->parent_->right_ = node;
                }
            }
            else
            {
                root_ = node;
            }
            old->left_->parent_ = node;
            if (old->right_)
            {
                old->right_->parent_ = node;
            }
            if (is_black_)
            {
                EraseColor(child, parent);
            }
            return;
        }
        parent = node->parent_;
        is_black_ = node->is_black_;
        if (child)
        {
            child->parent_ = parent;
        }
        if (parent)
        {
            if (parent->left_ == node)
            {
                parent->left_ = node;
            }
            else
            {
                parent->right_ = node;
            }
        }
        else
        {
            root_ = child;
        }
        if (is_black)
        {
            EraseColor(child, parent);
        }
    }
    void EraseColor(RBTreeNode *node, RBTreeNode *parent)
    {
        RBTreeNode *other = nullptr;
        while ((!node || node->rb_color == RB_BLACK) && node != root->rb_node)
        {
            if (parent->rb_left == node)
            {
                other = parent->rb_right;
                if (other->rb_color == RB_RED)
                {
                    other->rb_color = RB_BLACK;
                    parent->rb_color = RB_RED;
                    __rb_rotate_left(parent, root);
                    other = parent->rb_right;
                }
                if ((!other->rb_left || other->rb_left->rb_color == RB_BLACK) &&
                    (!other->rb_right || other->rb_right->rb_color == RB_BLACK))
                {
                    other->rb_color = RB_RED;
                    node = parent;
                    parent = node->rb_parent;
                }
                else
                {
                    if (!other->rb_right || other->rb_right->rb_color == RB_BLACK)
                    {
                        register rb_node_t *o_left;
                        if ((o_left = other->rb_left))
                            o_left->rb_color = RB_BLACK;
                        other->rb_color = RB_RED;
                        __rb_rotate_right(other, root);
                        other = parent->rb_right;
                    }
                    other->rb_color = parent->rb_color;
                    parent->rb_color = RB_BLACK;
                    if (other->rb_right)
                        other->rb_right->rb_color = RB_BLACK;
                    __rb_rotate_left(parent, root);
                    node = root->rb_node;
                    break;
                }
            }
            else
            {
                other = parent->rb_left;
                if (other->rb_color == RB_RED)
                {
                    other->rb_color = RB_BLACK;
                    parent->rb_color = RB_RED;
                    __rb_rotate_right(parent, root);
                    other = parent->rb_left;
                }
                if ((!other->rb_left || other->rb_left->rb_color == RB_BLACK) &&
                    (!other->rb_right || other->rb_right->rb_color == RB_BLACK))
                {
                    other->rb_color = RB_RED;
                    node = parent;
                    parent = node->rb_parent;
                }
                else
                {
                    if (!other->rb_left || other->rb_left->rb_color == RB_BLACK)
                    {
                        register rb_node_t *o_right;
                        if ((o_right = other->rb_right))
                            o_right->rb_color = RB_BLACK;
                        other->rb_color = RB_RED;
                        __rb_rotate_left(other, root);
                        other = parent->rb_left;
                    }
                    other->rb_color = parent->rb_color;
                    parent->rb_color = RB_BLACK;
                    if (other->rb_left)
                        other->rb_left->rb_color = RB_BLACK;
                    __rb_rotate_right(parent, root);
                    node = root->rb_node;
                    break;
                }
            }
        }
        if (node)
            node->rb_color = RB_BLACK;
    }

private:
    RBTreeNode *root_{nullptr};
    Compare compare_;
};

} // namespace mzx

#endif
