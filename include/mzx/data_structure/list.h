#ifndef __MZX_LIST_H__
#define __MZX_LIST_H__

#include <mzx/logger.h>

namespace mzx
{

class List;

class ListNode final
{
    friend List;

public:
    ListNode();
    ~ListNode();

public:
    bool IsLinked() const
    {
        return next_ != this;
    }

    const ListNode *Prev() const
    {
        return prev_;
    }
    ListNode *Prev()
    {
        return prev_;
    }

    const ListNode *Next() const
    {
        return next_;
    }
    ListNode *Next()
    {
        return next_;
    }

    void Insert(ListNode *prev, ListNode *next)
    {
        MZX_CHECK(prev != nullptr && next != nullptr);
        prev_ = prev;
        next_ = next;
        next->prev_ = this;
        prev->next_ = this;
    }
    ListNode *Erase()
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

class List final
{
public:
    List();
    ~List();

public:
    const ListNode *Begin() const
    {
        return head_.next_;
    }
    ListNode *Begin()
    {
        return head_.next_;
    }

    const ListNode *End() const
    {
        return &head_;
    }
    ListNode *End()
    {
        return &head_;
    }

    const ListNode *RBegin() const
    {
        return head_.prev_;
    }
    ListNode *RBegin()
    {
        return head_.prev_;
    }

    const ListNode *REnd() const
    {
        return &head_;
    }
    ListNode *REnd()
    {
        return &head_;
    }

    bool Empty() const
    {
        return head_.next_ == &head_;
    }

    void PushFront(ListNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->Insert(&head_, head_.next_);
    }
    void PushBack(ListNode *node)
    {
        MZX_CHECK(node != nullptr);
        node->Insert(head_.prev_, &head_);
    }
    ListNode *PopFront()
    {
        if (head_.next_ == &head_)
        {
            return nullptr;
        }
        auto *node = head_.next_;
        node->Erase();
        return node;
    }
    ListNode *PopBack()
    {
        if (head_.prev_ == &head_)
        {
            return nullptr;
        }
        auto *node = head_.prev_;
        node->Erase();
        return node;
    }
    ListNode *Erase(ListNode *node)
    {
        MZX_CHECK(node != nullptr);
        return node->Erase();
    }
    void Clear()
    {
        while (head_.next_ != &head_)
        {
            Erase(head_.next_);
        }
    }

private:
    ListNode head_;
};

} // namespace mzx

#endif
