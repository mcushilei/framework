#ifndef __KKLIST_H__
#define __KKLIST_H__

#include ".\app_cfg.h"
#include ".\list.h"

#define END_DEF_KLIST

#define KLIST(__NAME)           __NAME##_klist_t

#define KKLIST_ITEM(__NAME)     __NAME##_klist_item_t

#define KLIST_INIT(__NAME, __KLIST)                                             \
            __NAME##_klist_init((__KLIST))

#define KLIST_FIND(__NAME, __KLIST, __KEY)                                      \
            __NAME##_klist_find((__KLIST), (__KEY))

#define KLIST_INSERT(__NAME, __KLIST, __ITEM)                                   \
            __NAME##_klist_insert((__KLIST), (__ITEM))

#define KLIST_REMOVE(__NAME, __KLIST)                                           \
            __NAME##_klist_remove((__KLIST))

#define KLIST_SET_KEY(__NAME, __ITEM, __KEY)                                    \
            __NAME##_klist_set_key((__ITEM), (__KEY))

#define KLIST_GET_KEY(__NAME, __ITEM)                                           \
            __NAME##_klist_get_key((__ITEM))

#define KLIST_GET_PAYLOAD(__NAME, __ITEM)                                       \
            __NAME##_klist_get_payload((__ITEM))

#define KLIST_LENGTH(__NAME, __KLIST)                                           \
            __NAME##_list_get_length((__KLIST))

#define KLIST_MUTEX(__NAME, __KLIST)                                            \
            __NAME##_klist_mutex((__KLIST))

#define EXTERN_KLIST(__NAME, __TYPE, __DEC_TYPE, __MUTEX_TYPE)                  \
EXTERN_CLASS(__NAME##_klist_item_t)                                             \
    INHERIT(klist_item_t)                                                       \
    __TYPE                  tPayload;                                           \
END_EXTERN_CLASS(__NAME##_klist_item_t)                                         \
                                                                                \
EXTERN_CLASS(__NAME##_klist_t)                                                  \
    INHERIT(klist_t)                                                            \
    __MUTEX_TYPE            tMutex;                                             \
END_EXTERN_CLASS(__NAME##_klist_t)                                              \
                                                                                \
extern bool __NAME##_klist_init(__NAME##_klist_t *ptList);                      \
extern __NAME##_klist_item_t *__NAME##_klist_find(__NAME##_klist_t *ptList,     \
                                                  __DEC_TYPE tKey);             \
extern bool __NAME##_klist_insert(__NAME##_klist_t *ptList,                     \
                                  __NAME##_klist_item_t *ptItem);               \
extern __NAME##_klist_item_t *__NAME##_klist_remove(__NAME##_klist_t *ptList);  \
extern void __NAME##_klist_set_key(__NAME##_klist_item_t *ptItem, __DEC_TYPE tKey); \
extern __DEC_TYPE __NAME##_klist_get_key(__NAME##_klist_item_t *ptItem);        \
extern __TYPE *__NAME##_klist_get_payload(__NAME##_klist_item_t *ptItem);       \
extern __DEC_TYPE __NAME##_list_get_length(__NAME##_klist_t *ptList);           \
extern __MUTEX_TYPE *__NAME##_klist_mutex(__NAME##_klist_t *ptList);            \

#define DEF_KLIST(__NAME, __TYPE, __DEC_TYPE, __MUTEX_TYPE)                     \
DEF_CLASS(__NAME##_klist_item_t)                                                \
    INHERIT(klist_item_t)                                                       \
    __TYPE                      tPayload;                                       \
END_DEF_CLASS(__NAME##_klist_item_t)                                            \
                                                                                \
DEF_CLASS(__NAME##_klist_t)                                                     \
    INHERIT(klist_t)                                                            \
    __MUTEX_TYPE                tMutex;                                         \
END_DEF_CLASS(__NAME##_klist_t)                                                 \
                                                                                \
bool __NAME##_klist_init(__NAME##_klist_t *ptList)                              \
{                                                                               \
    return list_init((list_t *)ptList);                                         \
}                                                                               \
                                                                                \
__NAME##_klist_item_t *__NAME##_klist_find(__NAME##_klist_t *ptList,            \
                                           __DEC_TYPE tKey)                     \
{                                                                               \
    return (__NAME##_klist_item_t *)klist_find((klist_t *)ptList, tKey);        \
}                                                                               \
                                                                                \
bool __NAME##_klist_insert(__NAME##_klist_t *ptList,                            \
                           __NAME##_klist_item_t *ptItem)                       \
{                                                                               \
    return klist_insert((klist_t *)ptList, (klist_item_t *)ptItem);             \
}                                                                               \
                                                                                \
__NAME##_klist_item_t *__NAME##_klist_remove(__NAME##_klist_t *ptList)          \
{                                                                               \
    return (__NAME##_klist_item_t *)klist_remove((klist_t *)ptList);            \
}                                                                               \
                                                                                \
void __NAME##_klist_set_key(__NAME##_klist_item_t *ptItem, __DEC_TYPE tKey)     \
{                                                                               \
    klist_set_key((klist_item_t *)ptItem, tKey);                                \
}                                                                               \
                                                                                \
__DEC_TYPE __NAME##_klist_get_key(__NAME##_klist_item_t *ptItem)                \
{                                                                               \
    return klist_get_key((klist_item_t *)ptItem);                               \
}                                                                               \
                                                                                \
__TYPE *__NAME##_klist_get_payload(__NAME##_klist_item_t *ptItem)               \
{                                                                               \
    if (NULL == ptItem) {                                                       \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
    return &(((CLASS(__NAME##_klist_item_t) *)ptItem)->tPayload);               \
}                                                                               \
                                                                                \
__DEC_TYPE __NAME##_list_get_length(__NAME##_klist_t *ptList)                   \
{                                                                               \
    return list_get_length((list_t *)ptList);                                   \
}                                                                               \
                                                                                \
__MUTEX_TYPE *__NAME##_klist_mutex(__NAME##_klist_t *ptList)                    \
{                                                                               \
    if (NULL == ptList) {                                                       \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
    return &(((CLASS(__NAME##_klist_t) *)ptList)->tMutex);                      \
}                                                                               \

EXTERN_CLASS(klist_item_t)
    klist_item_t            *ptNext;
    uint32_t                wKey;
END_EXTERN_CLASS(klist_item_t)

EXTERN_CLASS(klist_t)
    klist_item_t            *ptHead;
    uint32_t                wCounter;
END_EXTERN_CLASS(klist_t)

extern klist_item_t *klist_find(klist_t *ptList, uint32_t wKey);
extern bool klist_insert(klist_t *ptList, klist_item_t *ptItem);
extern klist_item_t *klist_remove(klist_t *ptList);
extern void klist_set_key(klist_item_t *ptItem, uint32_t wKey);
extern uint32_t klist_get_key(klist_item_t *ptItem);

#endif
