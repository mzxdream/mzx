#include <mzx/data_structure/rbtree.h>

namespace mzx
{

void RBTreeNode::_ChangeChild(RBTreeNode *old_node, RBTreeNode *new_node, RBTreeNode *parent, RBTreeNode **root)
{
    if (parent)
    {
        if (parent->Left() == old_node)
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

void RBTreeNode::_RotateSetParents(RBTreeNode *old_node, RBTreeNode *new_node, RBTreeNode **root, bool is_black)
{
    RBTreeNode *parent = old_node->Parent();
    new_node->SetParentColor(old_node->ParentColor());
    old_node->SetParentColor(new_node, is_black);
    _ChangeChild(old_node, new_node, parent, root);
}

void RBTreeNode::_Insert(RBTreeNode *node, RBTreeNode **root)
{
    RBTreeNode *parent = node->Parent();
    RBTreeNode *gparent = nullptr;
    RBTreeNode *tmp = nullptr;
    while (true)
    {
        if (!parent)
        {
            node->SetParentColor(nullptr, true);
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
                tmp->SetParentColor(gparent, true);
                parent->SetParentColor(gparent, true);
                node = gparent;
                parent = node->Parent();
                node->SetParentColor(parent, false);
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
                    tmp->SetParentColor(parent, true);
                }
                parent->SetParentColor(node, false);
                parent = node;
                tmp = node->Right();
            }
            gparent->SetLeft(tmp);
            parent->SetRight(gparent);
            if (tmp)
            {
                tmp->SetParentColor(gparent, true);
            }
            _RotateSetParents(gparent, parent, root, false);
            break;
        }
        else
        {
            tmp = gparent->Left();
            if (tmp && tmp->IsRed())
            {
                tmp->SetParentColor(gparent, true);
                parent->SetParentColor(gparent, true);
                node = gparent;
                parent = node->Parent();
                node->SetParentColor(parent, false);
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
                    tmp->SetParentColor(parent, true);
                }
                parent->SetParentColor(node, false);
                parent = node;
                tmp = node->Left();
            }
            gparent->SetRight(tmp);
            parent->SetLeft(gparent);
            if (tmp)
            {
                tmp->SetParentColor(gparent, true);
            }
            _RotateSetParents(gparent, parent, root, false);
            break;
        }
    }
}

void RBTreeNode::_Erase(RBTreeNode *node, RBTreeNode **root)
{
    RBTreeNode *child = node->Right();
    RBTreeNode *tmp = node->Left();
    RBTreeNode *parent = nullptr;
    RBTreeNode *rebalance = nullptr;
    unsigned long pc = 0;
    if (!tmp)
    {
        pc = node->ParentColor();
        parent = RBTreeNode::Parent(pc);
        _ChangeChild(node, child, parent, root);
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
        _ChangeChild(node, tmp, parent, root);
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
        _ChangeChild(node, successor, tmp, root);
        if (child2)
        {
            successor->SetParentColor(pc);
            child2->SetParentColor(parent, true);
            rebalance = nullptr;
        }
        else
        {
            unsigned long pc2 = successor->ParentColor();
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
                    tmp1->SetParentColor(parent, true);
                    _RotateSetParents(parent, sibling, root, false);
                    sibling = tmp1;
                }
                tmp1 = sibling->Right();
                if (!tmp1 || tmp1->IsBlack())
                {
                    tmp2 = sibling->Left();
                    if (!tmp2 || tmp2->IsBlack())
                    {
                        sibling->SetParentColor(parent, false);
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
                        tmp1->SetParentColor(sibling, true);
                    }
                    tmp1 = sibling;
                    sibling = tmp2;
                }
                tmp2 = sibling->Left();
                parent->SetRight(tmp2);
                sibling->SetLeft(parent);
                tmp1->SetParentColor(sibling, true);
                if (tmp2)
                {
                    tmp2->SetParent(parent);
                }
                _RotateSetParents(parent, sibling, root, true);
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
                    tmp1->SetParentColor(parent, true);
                    _RotateSetParents(parent, sibling, root, false);
                    sibling = tmp1;
                }
                tmp1 = sibling->Left();
                if (!tmp1 || tmp1->IsBlack())
                {
                    tmp2 = sibling->Right();
                    if (!tmp2 || tmp2->IsBlack())
                    {
                        sibling->SetParentColor(parent, false);
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
                        tmp1->SetParentColor(sibling, true);
                    }
                    tmp1 = sibling;
                    sibling = tmp2;
                }
                tmp2 = sibling->Right();
                parent->SetLeft(tmp2);
                sibling->SetRight(parent);
                tmp1->SetParentColor(sibling, true);
                if (tmp2)
                {
                    tmp2->SetParent(parent);
                }
                _RotateSetParents(parent, sibling, root, true);
                break;
            }
        }
    }
}

void RBTreeNode::Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root)
{
    MZX_CHECK(link != nullptr && root != nullptr);
    SetParentColor(parent, false);
    left_ = nullptr;
    right_ = nullptr;
    *link = this;
    _Insert(this, root);
}

void RBTreeNode::Erase(RBTreeNode **root)
{
    MZX_CHECK(root != nullptr);
    _Erase(this, root);
}

} // namespace mzx
