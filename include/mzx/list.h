#ifndef __MZX_LIST_H__
#define __MZX_LIST_H__

namespace mzx {

struct ListHead
{
    ListHead *prev;
    ListHead *next;
};

#define MZX_LIST_HEAD_INIT(name) \
    { .prev = &(name), .next = &(name) }

#define MZX_LIST_HEAD(name) \
    mzx::ListHead name = MZX_LIST_HEAD_INIT(name)

#define MZX_INIT_LIST_HEAD(head) \
    do {\
        (head)->prev = (head);\
        (head)->next = (head);\
    } while (0)

#define MZX_LIST_INSERT(node, _pre, _nxt) \
    do {\
        auto prev = (_pre);\
        auto next = (_nxt);\
        next->prev = (node);\
        (node)->next = next;\
        (node)->prev = prev;\
        prev->next = (node);\
    } while (0)

#define MZX_LIST_PUSH_FRONT(node, head) \
    MZX_LIST_INSERT(node, head, (head)->next)

#define MZX_LIST_PUSH_BACK(node, head) \
    MZX_LIST_INSERT(node, (head)->prev, head)

#define MZX_LIST_ENTRY(node, type, member) \
    ({\
        const decltype(((type *)0)->member) *ptr = (node);\
        (type *)((char *)ptr - (std::size_t) &((type *)0)->member);\
    })

#define MZX_LIST_REMOVE_UNINIT(node) \
    do {\
        (node)->next->prev = (node)->prev;\
        (node)->prev->next = (node)->next;\
    } while (0)

#define MZX_LIST_REMOVE(node) \
    MZX_LIST_REMOVE_UNINIT(node); \
    MZX_INIT_LIST_HEAD(node)

#define MZX_LIST_FOREACH(it, head) \
    for (decltype(head) it = (head)->next; it != (head); it = it->next) 

#define MZX_LIST_FOREACH_REVERSE(it, head) \
    for (decltype(head) it = (head)->prev; it != (head); it = it->prev) 

}

#endif
