#include <mzx/data_structure/rbtree.h>

namespace mzx
{

RBTreeNode::RBTreeNode()
{
    parent_color_ = reinterpret_cast<ParentColorType>(this);
}
RBTreeNode::~RBTreeNode()
{
}

RBTreeNode *RBTreeNode::Prev()
{
    if (!IsLinked())
    {
        return nullptr;
    }
    auto *node = this;
    if (node->Left())
    {
        node = node->Left();
        while (node->Right())
        {
            node = node->Right();
        }
        return node;
    }
    RBTreeNode *parent = nullptr;
    while ((parent = node->Parent()) && node == parent->Left())
    {
        node = parent;
    }
    return parent;
}

RBTreeNode *RBTreeNode::Next()
{
    if (!IsLinked())
    {
        return nullptr;
    }
    auto *node = this;
    if (node->Right())
    {
        node = node->Right();
        while (node->Left())
        {
            node = node->Left();
        }
        return node;
    }
    RBTreeNode *parent = nullptr;
    while ((parent = node->Parent()) && node == parent->Right())
    {
        node = parent;
    }
    return parent;
}

void RBTreeNode::ChangeChild(RBTreeNode *new_node, RBTreeNode *parent, RBTreeNode **root)
{
    MZX_CHECK(root != nullptr);
    if (parent)
    {
        if (parent->Left() == this)
        {
            parent->SetLeft(new_node);
        }
        else
        {
            parent->SetRight(new_node);
        }
    }
    else
    {
        *root = new_node;
    }
}

void RBTreeNode::RotateSetParents(RBTreeNode *new_node, RBTreeNode **root, Color color)
{
    MZX_CHECK(new_node != nullptr && root != nullptr);
    RBTreeNode *parent = this->Parent();
    new_node->SetParentColor(this->ParentColor());
    this->SetParentColor(new_node, color);
    this->ChangeChild(new_node, parent, root);
}

void RBTreeNode::Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root)
{
    MZX_CHECK(link != nullptr && root != nullptr);
    SetParentColor(parent, Color::Red);
    left_ = nullptr;
    right_ = nullptr;
    *link = this;

    RBTreeNode *node = this;
    RBTreeNode *gparent = nullptr;
    RBTreeNode *tmp = nullptr;
    while (true)
    {
        if (!parent)
        {
            node->SetParentColor(nullptr, Color::Black);
            break;
        }
        if (parent->IsBlack())
        {
            break;
        }
        gparent = parent->Parent();
        tmp = gparent->Right();
        if (parent != tmp)
        {
            if (tmp && tmp->IsRed())
            {
                tmp->SetParentColor(gparent, Color::Black);
                parent->SetParentColor(gparent, Color::Black);
                node = gparent;
                parent = node->Parent();
                node->SetParentColor(parent, Color::Red);
                continue;
            }
            tmp = parent->Right();
            if (node == tmp)
            {
                tmp = node->Left();
                parent->SetRight(tmp);
                node->SetLeft(parent);
                if (tmp)
                {
                    tmp->SetParentColor(parent, Color::Black);
                }
                parent->SetParentColor(node, Color::Red);
                parent = node;
                tmp = node->Right();
            }
            gparent->SetLeft(tmp);
            parent->SetRight(gparent);
            if (tmp)
            {
                tmp->SetParentColor(gparent, Color::Black);
            }
            gparent->RotateSetParents(parent, root, Color::Red);
            break;
        }
        else
        {
            tmp = gparent->Left();
            if (tmp && tmp->IsRed())
            {
                tmp->SetParentColor(gparent, Color::Black);
                parent->SetParentColor(gparent, Color::Black);
                node = gparent;
                parent = node->Parent();
                node->SetParentColor(parent, Color::Red);
                continue;
            }
            tmp = parent->Left();
            if (node == tmp)
            {
                tmp = node->Right();
                parent->SetLeft(tmp);
                node->SetRight(parent);
                if (tmp)
                {
                    tmp->SetParentColor(parent, Color::Black);
                }
                parent->SetParentColor(node, Color::Red);
                parent = node;
                tmp = node->Left();
            }
            gparent->SetRight(tmp);
            parent->SetLeft(gparent);
            if (tmp)
            {
                tmp->SetParentColor(gparent, Color::Black);
            }
            gparent->RotateSetParents(parent, root, Color::Red);
            break;
        }
    }
}

void RBTreeNode::Erase(RBTreeNode **root)
{
    MZX_CHECK(root != nullptr);
    RBTreeNode *node = this;
    RBTreeNode *child = node->Right();
    RBTreeNode *tmp = node->Left();
    RBTreeNode *parent = nullptr;
    RBTreeNode *rebalance = nullptr;
    ParentColorType pc = 0;
    if (!tmp)
    {
        pc = node->ParentColor();
        parent = RBTreeNode::Parent(pc);
        node->ChangeChild(child, parent, root);
        if (child)
        {
            child->SetParentColor(pc);
            rebalance = nullptr;
        }
        else
        {
            rebalance = RBTreeNode::IsBlack(pc) ? parent : nullptr;
        }
        tmp = parent;
    }
    else if (!child)
    {
        pc = node->ParentColor();
        tmp->SetParentColor(pc);
        parent = RBTreeNode::Parent(pc);
        node->ChangeChild(tmp, parent, root);
        rebalance = nullptr;
        tmp = parent;
    }
    else
    {
        RBTreeNode *successor = child;
        RBTreeNode *child2 = nullptr;
        if (!tmp)
        {
            parent = successor;
            child2 = successor->Right();
        }
        else
        {
            do
            {
                parent = successor;
                successor = tmp;
                tmp = tmp->Left();
            } while (tmp);
            child2 = successor->Right();
            parent->SetLeft(child2);
            successor->SetRight(child);
            child->SetParent(successor);
        }
        tmp = node->Left();
        successor->SetLeft(tmp);
        tmp->SetParent(successor);
        pc = node->ParentColor();
        tmp = RBTreeNode::Parent(pc);
        node->ChangeChild(successor, tmp, root);
        if (child2)
        {
            successor->SetParentColor(pc);
            child2->SetParentColor(parent, Color::Black);
            rebalance = nullptr;
        }
        else
        {
            auto pc2 = successor->ParentColor();
            successor->SetParentColor(pc);
            rebalance = RBTreeNode::IsBlack(pc2) ? parent : nullptr;
        }
        // tmp = successor;
    }
    if (rebalance)
    {
        parent = rebalance;
        node = nullptr;
        RBTreeNode *sibling = nullptr;
        RBTreeNode *tmp1 = nullptr;
        RBTreeNode *tmp2 = nullptr;
        while (true)
        {
            sibling = parent->Right();
            if (node != sibling)
            {
                if (sibling->IsRed())
                {
                    tmp1 = sibling->Left();
                    parent->SetRight(tmp1);
                    sibling->SetLeft(parent);
                    tmp1->SetParentColor(parent, Color::Black);
                    parent->RotateSetParents(sibling, root, Color::Red);
                    sibling = tmp1;
                }
                tmp1 = sibling->Right();
                if (!tmp1 || tmp1->IsBlack())
                {
                    tmp2 = sibling->Left();
                    if (!tmp2 || tmp2->IsBlack())
                    {
                        sibling->SetParentColor(parent, Color::Red);
                        if (parent->IsRed())
                        {
                            parent->SetBlack();
                        }
                        else
                        {
                            node = parent;
                            parent = node->Parent();
                            if (parent)
                            {
                                continue;
                            }
                        }
                        break;
                    }
                    tmp1 = tmp2->Right();
                    sibling->SetLeft(tmp1);
                    tmp2->SetRight(sibling);
                    parent->SetRight(tmp2);
                    if (tmp1)
                    {
                        tmp1->SetParentColor(sibling, Color::Black);
                    }
                    tmp1 = sibling;
                    sibling = tmp2;
                }
                tmp2 = sibling->Left();
                parent->SetRight(tmp2);
                sibling->SetLeft(parent);
                tmp1->SetParentColor(sibling, Color::Black);
                if (tmp2)
                {
                    tmp2->SetParent(parent);
                }
                parent->RotateSetParents(sibling, root, Color::Black);
                break;
            }
            else
            {
                sibling = parent->Left();
                if (sibling->IsRed())
                {
                    tmp1 = sibling->Right();
                    parent->SetLeft(tmp1);
                    sibling->SetRight(parent);
                    tmp1->SetParentColor(parent, Color::Black);
                    parent->RotateSetParents(sibling, root, Color::Red);
                    sibling = tmp1;
                }
                tmp1 = sibling->Left();
                if (!tmp1 || tmp1->IsBlack())
                {
                    tmp2 = sibling->Right();
                    if (!tmp2 || tmp2->IsBlack())
                    {
                        sibling->SetParentColor(parent, Color::Red);
                        if (parent->IsRed())
                        {
                            parent->SetBlack();
                        }
                        else
                        {
                            node = parent;
                            parent = node->Parent();
                            if (parent)
                            {
                                continue;
                            }
                        }
                        break;
                    }
                    tmp1 = tmp2->Left();
                    sibling->SetRight(tmp1);
                    tmp2->SetLeft(sibling);
                    parent->SetLeft(tmp2);
                    if (tmp1)
                    {
                        tmp1->SetParentColor(sibling, Color::Black);
                    }
                    tmp1 = sibling;
                    sibling = tmp2;
                }
                tmp2 = sibling->Right();
                parent->SetLeft(tmp2);
                sibling->SetRight(parent);
                tmp1->SetParentColor(sibling, Color::Black);
                if (tmp2)
                {
                    tmp2->SetParent(parent);
                }
                parent->RotateSetParents(sibling, root, Color::Black);
                break;
            }
        }
    }
}

} // namespace mzx
