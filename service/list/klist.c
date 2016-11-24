#include ".\app_cfg.h"
#include ".\list.h"

DEF_CLASS(klist_item_t)
    klist_item_t           *ptNext;
    uint32_t                wKey;
END_DEF_CLASS(klist_item_t)

DEF_CLASS(klist_t)
    klist_item_t           *ptHead;
    uint32_t                wCounter;
END_DEF_CLASS(klist_t)

klist_item_t *klist_find(klist_t *ptList, uint32_t wKey)
{
    CLASS(klist_t) *pLIST = (CLASS(klist_t) *)ptList;
    
    klist_item_t *ptItem = NULL;

    if (NULL == ptList) {
        return NULL;
    }

    __LIST_ATOM_ACCESS(
        do {
            for (ptItem = pLIST->ptHead;
                 NULL != ptItem;
                 ptItem = ((CLASS(klist_item_t) *)ptItem)->ptNext) {
                // if current key is less or equal
                if (((CLASS(klist_item_t) *)ptItem)->wKey == wKey) {
                    break;
                }
            }
        } while (false);
    )

    return ptItem;
}

bool klist_insert(klist_t *ptList, klist_item_t *ptItem)
{
    CLASS(klist_t) *pLIST = (CLASS(klist_t) *)ptList;
    bool bResault = false;
    CLASS(klist_item_t) *ptCurrentItem = NULL;
    CLASS(klist_item_t) *ptNextItem    = NULL;

    if ((NULL == ptList) || (NULL == ptItem)) {
        return false;
    }

    __LIST_ATOM_ACCESS(
        do {
            ptCurrentItem = (CLASS(klist_item_t) *)pLIST->ptHead;
            if (NULL == ptCurrentItem) {
                bResault = list_add_head((list_t *)ptList, (list_node_t *)ptItem);
                break;
            }
            if (ptCurrentItem->wKey
            >   ((CLASS(klist_item_t) *)ptItem)->wKey) {
                bResault = list_add_head((list_t *)ptList, (list_node_t *)ptItem);
                break;
            }

            // if current key is big than fist item, then add current to head
            for (ptNextItem = (CLASS(klist_item_t) *)ptCurrentItem->ptNext;
                 NULL != ptNextItem;
                 ptNextItem = (CLASS(klist_item_t) *)ptCurrentItem->ptNext) {
                // if current key is less or equal
                if (ptNextItem->wKey
                >   ((CLASS(klist_item_t) *)ptItem)->wKey) {
                    break;
                }
                ptCurrentItem = (CLASS(klist_item_t) *)ptCurrentItem->ptNext;
            }
            bResault = list_insert_next((list_t *)ptList,
                                        (list_node_t *)ptCurrentItem,
                                        (list_node_t *)ptItem);
        } while (false);
    )

    return bResault;
}

klist_item_t *klist_remove(klist_t *ptList)
{
    if (NULL == ptList) {
        return NULL;
    }

    return (klist_item_t *)list_delete_head((list_t *)ptList);
}

void klist_set_key(klist_item_t *ptItem, uint32_t wKey)
{
    ((CLASS(klist_item_t) *)ptItem)->wKey = wKey;
}

uint32_t klist_get_key(klist_item_t *ptItem)
{
    if (NULL == ptItem) {
        return 0;
    }

    return ((CLASS(klist_item_t) *)ptItem)->wKey;
}
