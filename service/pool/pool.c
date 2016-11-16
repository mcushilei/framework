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


//! \note do not move this pre-processor statement to other places
#define __POOL_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\debug\debug.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define POOL_FREE_ITEM(__PPOOL, __PBLOCK)  do {         \
            ((pool_block_t *)(__PBLOCK))->ptNext =      \
                (pool_block_t *)(__PPOOL)->pFreeList;   \
            (__PPOOL)->pFreeList = (void *)__PBLOCK;    \
            (__PPOOL)->tCounter++;                      \
        } while (0)

/*============================ TYPES =========================================*/
DEF_STRUCTURE(pool_block_t)
    pool_block_t *      ptNext;
END_DEF_STRUCTURE(pool_block_t)

DEF_CLASS(pool_t)
    void *              pFreeList;
    __pool_uint_t       tBlockSize;
    __pool_uint_t       tSize;
    __pool_uint_t       tCounter;       //!< amount of block allocable
#ifdef __POOL_MUTEX_TYPE
    __POOL_MUTEX_TYPE   tMutex;
#endif
END_DEF_CLASS(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("POOL");

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool pool_init(pool_t *pPOOL, void *pMem, __pool_uint_t tPoolSize, __pool_uint_t tBlockSize)
{    
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;
    
    DEBUG_ASSERT_NOT_NULL(pPOOL);
    if (NULL == pPOOL) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(pMem);
    DEBUG_ASSERT_EQUAL_UINT(0, tPoolSize);
    DEBUG_ASSERT((sizeof(void *) <= tBlockSize), DEBUG_PRINT("tBlockSize < sizeof(void *)!", 0));
    if ((NULL == pMem) || (0 == tPoolSize) || (sizeof(void *) > tBlockSize)) {
        return false;
    }

    ptPOOL->pFreeList  = NULL;
    ptPOOL->tSize      = tPoolSize;
    ptPOOL->tBlockSize = tBlockSize;
    ptPOOL->tCounter   = 0;
    for (; tPoolSize; tPoolSize--) {
        POOL_FREE_ITEM(ptPOOL, pMem);
        pMem = (uint8_t *)pMem + ptPOOL->tBlockSize;
    }

    return true;
}

bool pool_deinit(pool_t *pPOOL)
{
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;

    DEBUG_ASSERT_NOT_NULL(pPOOL);
    if (NULL == pPOOL) {
        return false;
    }

    ptPOOL->pFreeList  = NULL;
    ptPOOL->tSize      = 0;
    ptPOOL->tBlockSize = 0;
    ptPOOL->tCounter   = 0;

    return true;
}

bool pool_free(pool_t *pPOOL, void *pBLOCK)
{    
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pPOOL);
    DEBUG_ASSERT_NOT_NULL(pBLOCK);
    if ((NULL == pPOOL) || (NULL == pBLOCK)) {
        return false;
    }

    DEBUG_ASSERT(ptPOOL->tCounter < ptPOOL->tSize,
                 DEBUG_PRINT("Terrible error: pool has been full!", 0));
    __POOL_ATOM_ACCESS(
        do {
            if (ptPOOL->tCounter
            >=  ptPOOL->tSize) {
                break;
            }
            POOL_FREE_ITEM(ptPOOL, pBLOCK);
            bResult = true;
        } while (0);
    )

    return bResult;
}

void *pool_new(pool_t *pPOOL)
{
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;
    void *pBLOCK = NULL;

    DEBUG_ASSERT_NOT_NULL(pPOOL);
    if (NULL == pPOOL) {
        return NULL;
    }

     __POOL_ATOM_ACCESS(
        do {
            if ((NULL == ptPOOL->pFreeList)  
            ||  (0 == ptPOOL->tCounter)) {
                DEBUG_MSG(POOL_DEBUG, "Pool is empty.");
                break; 
            }
            pBLOCK = ptPOOL->pFreeList;
            DEBUG_ASSERT_NOT_NULL(pBLOCK);
            ptPOOL->pFreeList  =
                (void *)((pool_block_t *)pBLOCK)->ptNext; 
            ((pool_block_t *)pBLOCK)->ptNext = NULL;
            ptPOOL->tCounter--;
        } while (false);
    )

    return pBLOCK;     
}    

__pool_uint_t pool_get_allocable_count(pool_t *pPOOL)
{
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;
    __pool_uint_t tCount;

    DEBUG_ASSERT_NOT_NULL(pPOOL);
    if (NULL == pPOOL) {
        return 0;
    }

    __POOL_ATOM_ACCESS(
        tCount = ptPOOL->tCounter;
    )

    return tCount;
}

#ifdef __POOL_MUTEX_TYPE
__POOL_MUTEX_TYPE *pool_get_mutex(pool_t *pPOOL)
{    
    CLASS(pool_t) *ptPOOL = (CLASS(pool_t) *)pPOOL;

    DEBUG_ASSERT_NOT_NULL(pPOOL);
    if (NULL == pPOOL)  {
        return NULL;   
    }

    return &(ptPOOL->tMutex);
}
#endif


/* EOF */
