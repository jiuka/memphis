/*
 *
 */


#ifndef LL_H
#define LL_H// LinkedList Logic

#define LL_INSERT(node,list)                                                \
    do {                                                                    \
        node->next = (list);                                                \
        (list) = (node);                                                    \
    } while (0)

#define LL_INSERT_KEY(node_,list)                              \
    do {                                                                    \
        typeof(list) _node = (node_);                   \
        typeof(_node) _ptr, _prev;                      \
        for (_ptr = (list), _prev = NULL; _ptr;         \
             _prev = _ptr, _ptr = _ptr->next            \
        ) {                                             \
            if (strcmp(_node->key, _ptr->key) > 0)      \
                break;                                  \
        }                                               \
        _node->next = _ptr;                             \
        if (_prev)                                      \
            _prev->next = _node;                        \
        else                                            \
            (list) = _node;                             \
    } while (0)

#define LL_INSERT_STR(node_,list)                              \
    do {                                                                    \
        typeof(list) _node = (node_);                   \
        typeof(_node) _ptr, _prev;                      \
        for (_ptr = (list), _prev = NULL; _ptr;         \
             _prev = _ptr, _ptr = _ptr->next            \
        ) {                                             \
            if (strcmp(_node->srt, _ptr->srt) > 0)      \
                break;                                  \
        }                                               \
        _node->next = _ptr;                             \
        if (_prev)                                      \
            _prev->next = _node;                        \
        else                                            \
            (list) = _node;                             \
    } while (0)

#define LL_INSERT_ID(node_,list)                              \
    do {                                                                    \
        typeof(list) _node = (node_);                   \
        typeof(_node) _ptr, _prev;                      \
        for (_ptr = (list), _prev = NULL; _ptr;         \
             _prev = _ptr, _ptr = _ptr->next            \
        ) {                                             \
            if (_node->id <_ptr->id) \
                break;                                  \
        }                                               \
        _node->next = _ptr;                             \
        if (_prev)                                      \
            _prev->next = _node;                        \
        else                                            \
            (list) = _node;                             \
    } while (0)

#define LL_APPEND(node_,list)                   \
    do {                                        \
        typeof(list) _node = (node_);           \
        typeof(_node) *_nextptr = &(list);      \
        typeof(_node) _prev = NULL;             \
        while (*_nextptr) {                     \
            _prev = *_nextptr;                  \
            _nextptr = &((*_nextptr)->next);    \
        }                                       \
        *_nextptr = _node;                      \
        _node->next = NULL;                     \
    } while (0)

#define LIST_FOREACH(iter,list) \
    for ((iter) = (list); (iter); (iter) = (iter)->next)

#define LL_SEARCH_ID(list,target,result)    \
    do {                                                        \
        LIST_FOREACH ((result), (list)) {                       \
            int i = (result)->id - (target);                 \
            if (i > 0)                                          \
                (result) = NULL;                                \
            if (i >= 0)                                         \
                break;                                          \
        }                                                       \
    } while (0)

#endif /* LL_H */

/*
 * vim: expandtab shiftwidth=4:
 */