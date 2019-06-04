#include <mzx/data_structure/list.h>

namespace mzx
{

ListNode::ListNode()
{
    prev_ = this;
    next_ = this;
}

ListNode::~ListNode()
{
    next_->prev_ = prev_;
    prev_->next_ = next_;
    prev_ = this;
    next_ = this;
}

List::List()
{
}

List::~List()
{
    Clear();
}

} // namespace mzx
