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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_STRUCTURE(pool_block_t)
    pool_block_t *      Next;
END_DEF_STRUCTURE(pool_block_t)

DEF_CLASS(pool_t)
    void *              FreeList;
    __pool_uint_t       Size;           //!< block size in byte.
    __pool_uint_t       Level;          //!< amount of block allocable
    __pool_uint_t       LevelMin;       //!< amount of block allocable
#ifdef __POOL_MUTEX_TYPE
    __POOL_MUTEX_TYPE   Mutex;
#endif
END_DEF_CLASS(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool pool_init(pool_t *PoolObj, void *PoolMem, __pool_uint_t PoolSize, __pool_uint_t BlockSize)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    void **ppBlockRef;

    if ((NULL == PoolObj) || (NULL == PoolMem) || (0 == PoolSize) || (sizeof(void *) > BlockSize)) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = PoolSize;
    Pool->Level     = PoolSize;
    Pool->LevelMin  = PoolSize;

    ppBlockRef = &Pool->FreeList;
    for (; PoolSize; PoolSize--) {
        *ppBlockRef = PoolMem;
        ppBlockRef  = &((pool_block_t *)(PoolMem))->Next;
        *ppBlockRef = NULL;
        PoolMem = (uint8_t *)PoolMem + BlockSize;
    }

    return true;
}

bool pool_deinit(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    if (NULL == PoolObj) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = 0;
    Pool->Level     = 0;
    Pool->LevelMin  = 0;

    return true;
}

bool pool_free(pool_t *PoolObj, void *Block)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    if ((NULL == PoolObj) || (NULL == Block)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        do {
            ((pool_block_t *)Block)->Next = (pool_block_t *)Pool->FreeList;
            Pool->FreeList = Block;
            Pool->Level++;
        } while (0);
    )

    return true;
}

void *pool_new(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;
    void *Block = NULL;

    if (NULL == PoolObj) {
        return NULL;
    }

     __POOL_ATOM_ACCESS(
        do {
            if (NULL == Pool->FreeList) {
                break; 
            }
            Block = Pool->FreeList;
            Pool->FreeList = ((pool_block_t *)Block)->Next; 
            ((pool_block_t *)Block)->Next = NULL;
            Pool->Level--;
            if (Pool->Level < Pool->LevelMin) {
                Pool->LevelMin = Pool->Level;
            }
        } while (false);
    )

    return Block;     
}    

__pool_uint_t pool_get_level(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    if (NULL == PoolObj) {
        return 0;
    }

    return Pool->Level;
}

__pool_uint_t pool_get_min_level(pool_t *PoolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    if (NULL == PoolObj) {
        return 0;
    }

    return Pool->LevelMin;
}

#ifdef __POOL_MUTEX_TYPE
__POOL_MUTEX_TYPE *pool_get_mutex(pool_t *PoolObj)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)PoolObj;

    if (NULL == PoolObj)  {
        return NULL;   
    }

    return &(Pool->Mutex);
}
#endif


/* EOF */
