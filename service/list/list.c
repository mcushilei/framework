/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify it   *
 *  under the terms of the GNU Lesser General Public License as published     *
 *  by the Free Software Foundation; either version 3 of the License, or      *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful, but       *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
 *  General Public License for more details.                                  *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this program; if not, see http://www.gnu.org/licenses/.        *
*******************************************************************************/




/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(list_item_t)
    list_item_t            *ptNext;
END_DEF_CLASS(list_item_t)

DEF_CLASS(list_t)
    list_item_t            *ptHead;
    uint32_t                wCounter;
END_DEF_CLASS(list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
bool list_init(list_t *pLIST)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;
    
    if (NULL == pLIST) {
        return false;
    }

    ptLIST->ptHead   = NULL;
    ptLIST->wCounter = 0;

    return true;
}

// add one item to head
bool list_add_head(list_t *pLIST, list_item_t *pITEM)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;

    if (NULL == pLIST) {
        return false;
    }

    __LIST_ATOM_ACCESS(
        ((CLASS(list_item_t) *)pITEM)->ptNext =
            ptLIST->ptHead;
        ptLIST->ptHead = pITEM;
        ptLIST->wCounter++;
    )

    return true;
}

// delete one item frome head
list_item_t *list_delete_head(list_t *pLIST)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;
    list_item_t *pITEM = NULL;

    if (NULL == pLIST) {
        return NULL;
    }

    __LIST_ATOM_ACCESS(
        do {
            if (NULL == ptLIST->ptHead) {
                break;
            }
            pITEM = ptLIST->ptHead;
            ptLIST->ptHead =
                ((CLASS(list_item_t) *)pITEM)->ptNext;
            ((CLASS(list_item_t) *)pITEM)->ptNext = NULL;
            if (ptLIST->wCounter) {
                ptLIST->wCounter--;
            }
        } while (false);
    )

    return pITEM;
}

// insert one item after another one
bool list_insert_next(list_t *pLIST, list_item_t *pPOSITION, list_item_t *pITEM)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;
    CLASS(list_item_t) *ptPOSITION = (CLASS(list_item_t) *)pPOSITION;

    if ((NULL == pLIST) || (NULL == pPOSITION) || (NULL == pITEM)) {
        return false;
    }

    __LIST_ATOM_ACCESS(
        ((CLASS(list_item_t) *)pITEM)->ptNext =
            ptPOSITION->ptNext;
        ptPOSITION->ptNext = pITEM;
        ptLIST->wCounter++;
    )

    return true;
}

// remove the next from one item
list_item_t *list_remove_next(list_t *pLIST, list_item_t *pPOSITION)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;
    CLASS(list_item_t) *ptPOSITION = (CLASS(list_item_t) *)pPOSITION;
    
    list_item_t *pITEM = NULL;

    if ((NULL == pLIST) || (NULL == pPOSITION)) {
        return NULL;
    }

    __LIST_ATOM_ACCESS(
        do {
            if (NULL == ptPOSITION->ptNext) {
                break;
            }
            pITEM = ptPOSITION->ptNext;
            ptPOSITION->ptNext =
                ((CLASS(list_item_t) *)pITEM)->ptNext;
            ((CLASS(list_item_t) *)pITEM)->ptNext = NULL;
            if (ptLIST->wCounter) {
                ptLIST->wCounter--;
            }
        } while (false);
    )

    return pITEM;
}

uint32_t list_get_length(list_t *pLIST)
{
    CLASS(list_t) *ptLIST = (CLASS(list_t) *)pLIST;
    uint32_t wCount;

    if (NULL == pLIST) {
        return 0;
    }

    __LIST_ATOM_ACCESS(
        wCount = ptLIST->wCounter;
    )

    return wCount;
}
