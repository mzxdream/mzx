#include <mzx/data_structure/rbtree.h>

namespace mzx
{

void RBTreeNode::Insert(RBTreeNode *parent, RBTreeNode **link, RBTreeNode **root)
{
    MZX_CHECK(parent != nullptr && link != nullptr && root != nullptr);
    parent_color_ = static_cast<unsigned long>(parent); // default color is red
    left_ = nullptr;
    right_ = nullptr;
    *link = this;

    RBTreeNode *gparent = nullptr;
    RBTreeNode *tmp = nullptr;
    while (true)
    {
        if (!parent)
        {

            break;
        }
    }
}

} // namespace mzx
