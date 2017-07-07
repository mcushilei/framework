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
    pool_block_t*   pNext;
END_DEF_STRUCTURE(pool_block_t)

//! ADT: list stack.
DEF_CLASS(pool_t)
    void*           pFreeList;
    pool_uint_t     BlockSize;      //!< size of one block in BYTE.
    pool_uint_t     Size;           //!< pool size.
    pool_uint_t     Level;          //!< pool level
    pool_uint_t     LevelMin;       //!< minimum pool level
END_DEF_CLASS(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool pool_init(void* pPoolObj, pool_uint_t blockSize)
{    
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;

    if (NULL == pPoolObj || 0u == blockSize || 0 != (blockSize % sizeof(void*))) {
        return false;
    }

    Pool->pFreeList = NULL;
    Pool->BlockSize = blockSize;
    Pool->Size      = 0u;
    Pool->Level     = 0u;
    Pool->LevelMin  = 0u;

    return true;
}

bool pool_deinit(void* pPoolObj)
{
    return true;
}

bool pool_add_memory(void* pPoolObj, void* pMem, pool_uint_t num)
{
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;

    if ((NULL == pPoolObj) || (NULL == pMem)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        for (; num; num--) {
            ((pool_block_t*)pMem)->pNext = (pool_block_t*)Pool->pFreeList;
            Pool->pFreeList = pMem;
            pMem = (uint8_t*)pMem + Pool->BlockSize;
        } while (0);
        Pool->Level += num;
    )


    return true;
}

bool pool_free(void* pPoolObj, void* pMem)
{    
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;

    if ((NULL == pPoolObj) || (NULL == pMem)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        do {
            ((pool_block_t*)pMem)->pNext = (pool_block_t*)Pool->pFreeList;
            Pool->pFreeList = pMem;
            Pool->Level++;
        } while (0);
    )

    return true;
}

void* pool_new(void* pPoolObj)
{
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;
    void* pMem = NULL;

    if (NULL == pPoolObj) {
        return NULL;
    }

     __POOL_ATOM_ACCESS(
        do {
            if (NULL == Pool->pFreeList) {
                break; 
            }
            pMem = Pool->pFreeList;
            Pool->pFreeList = ((pool_block_t*)pMem)->pNext; 
            ((pool_block_t*)pMem)->pNext = NULL;
            Pool->Level--;
            if (Pool->Level < Pool->LevelMin) {
                Pool->LevelMin = Pool->Level;
            }
        } while (0);
    )

    return pMem;     
}    

pool_uint_t pool_get_level(void* pPoolObj)
{
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;

    if (NULL == pPoolObj) {
        return 0;
    }

    return Pool->Level;
}

pool_uint_t pool_get_min_level(void* pPoolObj)
{
    CLASS(pool_t)* Pool = (CLASS(pool_t)*)pPoolObj;

    if (NULL == pPoolObj) {
        return 0;
    }

    return Pool->LevelMin;
}


/* EOF */
