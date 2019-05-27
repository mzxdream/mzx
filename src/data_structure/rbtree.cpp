#include <mzx/data_structure/rbtree.h>

namespace mzx
{

static void RBTreeInsert(RBTreeNode *node, RBTreeNode **root)
{
    MZX_CHECK(node != nullptr && root != nullptr);
    RBTreeNode *parent = node->Parent();
    RBTreeNode *gparent = nullptr;
    RBTreeNode *tmp = nullptr;
    while (true)
    {
        if (!parent)
        {
            node->SetParentAndBlackColor(nullptr);
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
                tmp->SetParentAndBlackColor(gparent);
                parent->SetParentAndBlackColor(gparent);
                node = gparent;
                parent = node->Parent();
                node->SetParentAndRedColor(parent);
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
                    tmp->SetParentAndBlackColor(parent);
                }
                parent->SetParentAndRedColor(node);
                parent = node;
                tmp = node->Right();
            }
            gparent->SetLeft(tmp);
            parent->SetRight(gparent);
            if (tmp)
            {
                tmp->SetParentAndBlackColor(gparent);
            }
            RBTreeRotateSetParents(gparent, parent, root, RB_RED);
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
