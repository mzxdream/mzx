#ifndef __MZX_LIST_SAFE_NODE_H__
#define __MZX_LIST_SAFE_NODE_H__

#include <mzx/data_structure/list.h>

namespace mzx
{

template <typename T>
class ListSafeNode final
{
public:
    explicit ListSafeNode(T *data)
        : raw_data_(data)
        , ref_count_(1)
    {
        MZX_CHECK(raw_data_ != nullptr);
    }
    ~ListSafeNode()
    {
        MZX_CHECK(raw_data_ == nullptr && ref_count_ == 0);
    }
    const ListNode *ListLink() const
    {
        return &list_link_;
    }
    MZX_NON_CONST_OVERLOAD(ListLink);
    T *Get() const
    {
        return raw_data_;
    }

    void IncrRef()
    {
        ++ref_count_;
    }
    void DecrRef()
    {
        MZX_CHECK(ref_count_ > 0);
        if (--ref_count_ == 0)
        {
            delete this;
        }
    }
    T *Detach()
    {
        if (raw_data_ != nullptr)
        {
            auto *data = raw_data_;
            raw_data_ = nullptr;
            DecrRef();
            return data;
        }
        return nullptr;
    }

private:
    T *raw_data_{nullptr};
    int ref_count_{0};
    ListNode list_link_;
};

} // namespace mzx

#endif
