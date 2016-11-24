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
            ((pool_block_t *)(__PBLOCK))->Next =      \
                (pool_block_t *)(__PPOOL)->FreeList;   \
            (__PPOOL)->FreeList = (void *)__PBLOCK;    \
            (__PPOOL)->Counter++;                      \
        } while (0)

/*============================ TYPES =========================================*/
DEF_STRUCTURE(pool_block_t)
    pool_block_t *      Next;
END_DEF_STRUCTURE(pool_block_t)

DEF_CLASS(pool_t)
    void *              FreeList;
    __pool_uint_t       BlockSize;
    __pool_uint_t       Size;
    __pool_uint_t       Counter;       //!< amount of block allocable
#ifdef __POOL_MUTEX_TYPE
    __POOL_MUTEX_TYPE   Mutex;
#endif
END_DEF_CLASS(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("POOL");

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool pool_init(pool_t *PoolObj, void *Memory, __pool_uint_t Size, __pool_uint_t BlockSize)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    
    DEBUG_ASSERT_NOT_NULL(PoolObj);
    DEBUG_ASSERT_NOT_NULL(Memory);
    DEBUG_ASSERT_EQUAL_UINT(0, Size);
    DEBUG_ASSERT(sizeof(void *) <= BlockSize);
    if ((NULL == PoolObj) || (NULL == Memory) || (0 == Size) || (sizeof(void *) > BlockSize)) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = Size;
    Pool->BlockSize = BlockSize;
    Pool->Counter   = 0;
    for (; Size; Size--) {
        POOL_FREE_ITEM(Pool, Memory);
        Memory = (uint8_t *)Memory + Pool->BlockSize;
    }

    return true;
}

bool pool_deinit(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    DEBUG_ASSERT_NOT_NULL(PoolObj);
    if (NULL == PoolObj) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = 0;
    Pool->BlockSize = 0;
    Pool->Counter   = 0;

    return true;
}

bool pool_free(pool_t *PoolObj, void *Block)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(PoolObj);
    DEBUG_ASSERT_NOT_NULL(Block);
    if ((NULL == PoolObj) || (NULL == Block)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        do {
            if (Pool->Counter
            >=  Pool->Size) {
                break;
            }
            POOL_FREE_ITEM(Pool, Block);
            bResult = true;
        } while (0);
    )

    return bResult;
}

void *pool_new(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    void *Block = NULL;

    DEBUG_ASSERT_NOT_NULL(PoolObj);
    if (NULL == PoolObj) {
        return NULL;
    }

     __POOL_ATOM_ACCESS(
        do {
            if ((NULL == Pool->FreeList)  
            ||  (0 == Pool->Counter)) {
                break; 
            }
            Block = Pool->FreeList;
            Pool->FreeList  =
                (void *)((pool_block_t *)Block)->Next; 
            ((pool_block_t *)Block)->Next = NULL;
            Pool->Counter--;
        } while (false);
    )

    return Block;     
}    

__pool_uint_t pool_get_allocable_count(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    __pool_uint_t tCount;

    DEBUG_ASSERT_NOT_NULL(PoolObj);
    if (NULL == PoolObj) {
        return 0;
    }

    __POOL_ATOM_ACCESS(
        tCount = Pool->Counter;
    )

    return tCount;
}

#ifdef __POOL_MUTEX_TYPE
__POOL_MUTEX_TYPE *pool_get_mutex(pool_t *PoolObj)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    DEBUG_ASSERT_NOT_NULL(PoolObj);
    if (NULL == PoolObj)  {
        return NULL;   
    }

    return &(Pool->Mutex);
}
#endif


/* EOF */
