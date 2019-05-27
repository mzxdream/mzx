#include <mzx/data_structure/rbtree.h>

namespace mzx
{

static void RBTreeRotateSetParents(RBTreeNode *old_node, RBTreeNode *new_node, RBTreeNode **root, bool is_black)
{
    // MZX_CHECK(old_node != nullptr && new_node != nullptr && root != nullptr);
    RBTreeNode *parent = old_node->Parent();
    new_node->SetParentColor(old_node->ParentColor());
    old_node->SetParentColor(new_node, is_black);
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

static void RBTreeInsert(RBTreeNode *node, RBTreeNode **root)
{
    // MZX_CHECK(node != nullptr && root != nullptr);
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
            RBTreeRotateSetParents(gparent, parent, root, false);
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
            RBTreeRotateSetParents(gparent, parent, root, false);
            break;
        }
    }
}

void RBTreeNode::Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root)
{
    MZX_CHECK(parent != nullptr && link != nullptr && root != nullptr);
    SetParentAndRedColor(parent);
    left_ = nullptr;
    right_ = nullptr;
    *link = this;
    RBTreeInsert(this, root);
}

} // namespace mzx
