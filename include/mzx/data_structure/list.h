#ifndef __MZX_LIST_H__
#define __MZX_LIST_H__

#include <mzx/logger.h>
#include <mzx/macro_util.h>

namespace mzx
{

class ListNode final
{
public:
    ListNode()
    {
        prev_ = this;
        next_ = this;
    }
    ~ListNode()
    {
        Unlink();
    }

public:
    inline bool IsLinked() const
    {
        return next_ != this;
    }
    inline ListNode *Prev() const
    {
        return prev_;
    }
    inline ListNode *Next() const
    {
        return next_;
    }
    inline void InsertInto(ListNode *prev, ListNode *next)
    {
        MZX_CHECK(prev != nullptr && next != nullptr);
        prev_ = prev;
        next_ = next;
        next->prev_ = this;
        prev->next_ = this;
    }
    inline void PushFront(ListNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->InsertInto(this, next_);
    }
    inline void PushBack(ListNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->InsertInto(prev_, this);
    }
    inline ListNode *Unlink()
    {
        auto *next = next_;
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = this;
        next_ = this;
        return next;
    }

private:
    ListNode *prev_{nullptr};
    ListNode *next_{nullptr};
};

#define MZX_LIST_ENTRY(node, type, member) MZX_CONTAINER_OF(node, type, member)

} // namespace mzx

#endif
