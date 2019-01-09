#ifndef __MZX_LIST_H__
#define __MZX_LIST_H__

#include <mzx/logger.h>

namespace mzx {

class ListHead final
{
public:
    ListHead()
    {
        prev_ = this;
        next_ = this;
    }
    ~ListHead()
    {
        Unlink();
    }
public:
    inline bool Empty() const
    {
        return next_ == this;
    }
    inline ListHead * Prev() const
    {
        return prev_;
    }
    inline ListHead * Next() const
    {
        return next_;
    }
public:
    inline void InsertInto(ListHead *prev, ListHead *next)
    {
        MZX_CHECK(prev != nullptr && next != nullptr);
        prev_ = prev;
        next_ = next;
        next->prev_ = this;
        prev->next_ = this;
    }
    inline void PushIntoFront(ListHead *head)
    {
        MZX_CHECK(head != nullptr);
        InsertInto(head, head->next_);
    }
    inline void PushIntoBack(ListHead *head)
    {
        MZX_CHECK(head != nullptr);
        InsertInto(head->prev_, head);
    }
    inline void Unlink()
    {
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = this;
        next_ = this;
    }
private:
    ListHead *prev_;
    ListHead *next_;
};

#define MZX_LIST_ENTRY(node, type, member) \
    ({\
        const decltype(((type *)0)->member) *ptr = (node);\
        (type *)((char *)ptr - (std::size_t) &((type *)0)->member);\
    })

}

#endif
