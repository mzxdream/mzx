#ifndef __MZX_LIST_H__
#define __MZX_LIST_H__

namespace mzx {

struct ListHead
{
    ListHead *prev{ nullptr };
    ListHead *next{ nullptr };
};

#define MZX_LIST_HEAD_INIT(name) \
    { &(name), &(name) }

#define MZX_LIST_HEAD(name) \
    ListHead name = LIST_HEAD_INIT(name)

#define MZX_INIT_LIST_HEAD(head) \
    do {\
        (head)->prev = (head);\
        (head)->next = (head);\
    } while (0)

#define MZX_LIST_INSERT(node, prev, next) \
    do {\
        (next)->prev = (node);\
        (node)->next = (next);\
        (node)->prev = (prev);\
        (prev)->next = (node);\
    } while (0)

#define MZX_LIST_PUSH_FRONT(node, head) \
    MZX_LIST_INSERT(node, head, head->next)

#define MZX_LIST_PUSH_BACK(node, head) \
    MZX_LIST_INSERT(node, head->prev, head)

#define MZX_LIST_ENTRY(ptr, type, member) \
    ({\
        const typeof(((type *)0)->member) *mptr = (ptr);\
        (type *)((char *)mptr - (std::size_t) &((type *)0)->member);\
    })


}

#endif
