/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify it   *
 *  under the terms of the GNU Lesser General Public License as published     *
 *  by the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful, but       *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  General Public License for more details.                                  *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with ptQUEUE program; if not, write to the Free Software Foundation,   *
 *  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
*******************************************************************************/

//! \note do not move ptQUEUE pre-processor statement to other places
#define __QUEUE_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\string\string.h"
#include "..\debug\debug.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(queue_t)
    void *              pBuffer;
    __queue_uint_t      tObjSize;
    __queue_uint_t      tSize;
    __queue_uint_t      tHead;
    __queue_uint_t      tTail;
    __queue_uint_t      tCounter;
    __queue_uint_t      tPeek;
    __queue_uint_t      tPeekCounter;
#ifdef __QUEUE_MUTEX_TYPE
    __QUEUE_MUTEX_TYPE  tMutex;
#endif
END_DEF_CLASS(queue_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("QUEUE");

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool queue_init(queue_t *pQUEUE, void *pBuffer, __queue_uint_t tSize, __queue_uint_t tObjSize)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    
    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    DEBUG_ASSERT_NOT_NULL(pBuffer);
    DEBUG_ASSERT(tSize != 0, DEBUG_PRINT("Queue buffer size must not be ZERO!"));
    if (NULL == pQUEUE || NULL == pBuffer || 0 == tSize) {
        return false;
    }

    ptQUEUE->pBuffer  = pBuffer;
    ptQUEUE->tObjSize = tObjSize;
    ptQUEUE->tSize    = tSize;
    ptQUEUE->tHead    = 0;
    ptQUEUE->tTail    = 0;
    ptQUEUE->tPeek    = 0;
    ptQUEUE->tCounter = 0;
    ptQUEUE->tPeekCounter = 0;

    return true;
}

bool queue_deinit(queue_t *pQUEUE)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return false;
    }

    ptQUEUE->pBuffer  = NULL;
    ptQUEUE->tObjSize = 0;
    ptQUEUE->tSize    = 0;
    ptQUEUE->tHead    = 0;
    ptQUEUE->tTail    = 0;
    ptQUEUE->tPeek    = 0;
    ptQUEUE->tCounter = 0;
    ptQUEUE->tPeekCounter = 0;

    return true;
}

bool queue_enqueue(queue_t *pQUEUE, void *ptObj)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(ptQUEUE->pBuffer);
    if (NULL == ptQUEUE->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((ptQUEUE->tHead ==  ptQUEUE->tTail)
            &&  (0 != ptQUEUE->tCounter)) {
                DEBUG_MSG(QUEUE_DEBUG, "Queue is full.");
                break;
            }

            switch (ptQUEUE->tObjSize) {
                case 1:
                    ((uint8_t *)(ptQUEUE->pBuffer))[
                                 ptQUEUE->tTail] = *(uint8_t *)ptObj;
                    break;
                case 2:
                    ((uint16_t *)(ptQUEUE->pBuffer))[
                                  ptQUEUE->tTail] = *(uint16_t *)ptObj;
                    break;
                case 4:
                    ((uint32_t *)(ptQUEUE->pBuffer))[
                                  ptQUEUE->tTail] = *(uint32_t *)ptObj;
                    break;
                default:
                    mem_copy((uint8_t *)ptQUEUE->pBuffer + (ptQUEUE->tObjSize) * (ptQUEUE->tTail),
                             ptObj,
                             ptQUEUE->tObjSize);
                    break;
            }
            ptQUEUE->tTail++;
            if (ptQUEUE->tTail >= ptQUEUE->tSize) {
                ptQUEUE->tTail = 0;
            }
            ptQUEUE->tCounter++;
            ptQUEUE->tPeekCounter++;
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

bool queue_dequeue(queue_t *pQUEUE, void *ptObj)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(ptQUEUE->pBuffer);
    if (NULL == ptQUEUE->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((ptQUEUE->tHead ==  ptQUEUE->tTail)
            &&  (0 == ptQUEUE->tCounter)) {
                DEBUG_MSG(QUEUE_DEBUG, "Queue is empty.");
                break;
            }

            if (NULL != ptObj) {
                switch (ptQUEUE->tObjSize) {
                    case 1:
                        *(uint8_t *)ptObj = ((uint8_t *)(ptQUEUE->pBuffer))[
                                                         ptQUEUE->tHead];
                        break;
                    case 2:
                        *(uint16_t *)ptObj = ((uint16_t *)(ptQUEUE->pBuffer))[
                                                           ptQUEUE->tHead];
                        break;
                    case 4:
                        *(uint32_t *)ptObj = ((uint32_t *)(ptQUEUE->pBuffer))[
                                                           ptQUEUE->tHead];
                        break;
                    default:
                        mem_copy(ptObj,
                                 (uint8_t *)ptQUEUE->pBuffer + (ptQUEUE->tObjSize) * (ptQUEUE->tHead),
                                 ptQUEUE->tObjSize);
                        break;
                }
            }
            ptQUEUE->tHead++;
            if (ptQUEUE->tHead >= ptQUEUE->tSize) {
                ptQUEUE->tHead = 0;
            }
            ptQUEUE->tCounter--;
            ptQUEUE->tPeek = ptQUEUE->tHead;
            ptQUEUE->tPeekCounter = ptQUEUE->tCounter;
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

bool queue_peek(queue_t *pQUEUE, void *ptObj)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(ptQUEUE->pBuffer);
    if (NULL == ptQUEUE->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((ptQUEUE->tPeek ==  ptQUEUE->tTail)
            &&  (0 == ptQUEUE->tPeekCounter)) {
                DEBUG_MSG(QUEUE_DEBUG, "Queue has been peeked all.");
                break;
            }
            if (NULL != ptObj) {
                switch (ptQUEUE->tObjSize) {
                    case 1:
                        *(uint8_t *)ptObj = ((uint8_t *)(ptQUEUE->pBuffer))[
                                                         ptQUEUE->tPeek];
                        break;
                    case 2:
                        *(uint16_t *)ptObj = ((uint16_t *)(ptQUEUE->pBuffer))[
                                                           ptQUEUE->tPeek];
                        break;
                    case 4:
                        *(uint32_t *)ptObj = ((uint32_t *)(ptQUEUE->pBuffer))[
                                                           ptQUEUE->tPeek];
                        break;
                    default:
                        *(__queue_uint_t *)ptObj = ((__queue_uint_t *)(ptQUEUE->pBuffer))[
                                                           ptQUEUE->tPeek];
                        break;
                }
            }
            ptQUEUE->tPeek++;
            ptQUEUE->tPeekCounter--;
            if (ptQUEUE->tPeek >= ptQUEUE->tSize) {
                ptQUEUE->tPeek = 0;
            }
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

void queue_get_all_peeked(queue_t *pQUEUE)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return ;
    }

    __QUEUE_ATOM_ENTER();
        ptQUEUE->tHead = ptQUEUE->tPeek;
        ptQUEUE->tCounter = ptQUEUE->tPeekCounter;
    __QUEUE_ATOM_EXIT();
}

void queue_reset_peek(queue_t *pQUEUE)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return ;
    }

    __QUEUE_ATOM_ENTER();
        ptQUEUE->tPeek = ptQUEUE->tHead;
        ptQUEUE->tPeekCounter = ptQUEUE->tCounter;
    __QUEUE_ATOM_EXIT();
}

__queue_uint_t queue_get_object_count(queue_t *pQUEUE)
{
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    __queue_uint_t tCounter;

    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE) {
        return 0;
    }

    __QUEUE_ATOM_ENTER();
        tCounter = ptQUEUE->tCounter;
    __QUEUE_ATOM_EXIT();

    return tCounter;
}

#ifdef __QUEUE_MUTEX_TYPE
__QUEUE_MUTEX_TYPE *queue_get_mutex(queue_t *pQUEUE)
{    
    CLASS(queue_t) *ptQUEUE = (CLASS(queue_t) *)pQUEUE;
    
    DEBUG_ASSERT_NOT_NULL(pQUEUE);
    if (NULL == pQUEUE)  {
        return NULL;   
    }

    return &(ptQUEUE->tMutex);
}
#endif

