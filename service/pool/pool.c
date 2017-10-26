/*******************************************************************************
 *  Copyright(C)2015-2017 by Dreistein<mcu_shilei@hotmail.com>                *
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
//! \note   this structure should be inherited by the memory block you want to be managed by pool.
//! @{
DEF_STRUCTURE(pool_block_t)
    pool_block_t   *Next;
END_DEF_STRUCTURE(pool_block_t)
//! @}

//! ADT: list stack.
DEF_STRUCTURE(pool_t)
    void           *FreeList;
    size_t          BlockSize;      //!< size of one memory block.
    pool_uint_t     Size;           //!< pool size.
    pool_uint_t     Level;          //!< pool level
    pool_uint_t     LevelMin;       //!< minimum pool level
END_DEF_STRUCTURE(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief  init an empty pool.
 */
bool pool_init(void *obj, pool_uint_t blockSize)
{    
    pool_t *Pool = (pool_t *)obj;

    //! block size should be mulitple size of pointer.
    if (NULL == obj || 0u == blockSize || 0u != (blockSize % sizeof(void *))) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->BlockSize = blockSize;
    Pool->Size      = 0u;
    Pool->Level     = 0u;
    Pool->LevelMin  = 0u;

    return true;
}

bool pool_deinit(void *obj)
{
    return true;
}

/*! \brief  add memory to pool.
 */
bool pool_add_memory(void *obj, void *pMem, pool_uint_t num)
{
    pool_t *Pool = (pool_t *)obj;

    if ((NULL == obj) || (NULL == pMem)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        for (; num; num--) {
            ((pool_block_t *)pMem)->Next = (pool_block_t *)Pool->FreeList;
            Pool->FreeList = pMem;
            pMem = (char *)pMem + Pool->BlockSize;
        }
        Pool->Level     += num;
        Pool->LevelMin  += num;
    )


    return true;
}

bool pool_free(void* obj, void* pMem)
{    
    pool_t *Pool = (pool_t *)obj;

    if ((NULL == obj) || (NULL == pMem)) {
        return false;
    }

    __POOL_ATOM_ACCESS(
        do {
            ((pool_block_t *)pMem)->Next = (pool_block_t *)Pool->FreeList;
            Pool->FreeList = pMem;
            Pool->Level++;
        } while (0);
    )

    return true;
}

void *pool_new(void *obj)
{
    pool_t *Pool = (pool_t *)obj;
    void *pMem = NULL;

    if (NULL == obj) {
        return NULL;
    }

     __POOL_ATOM_ACCESS(
        do {
            if (NULL == Pool->FreeList) {
                break; 
            }
            pMem = Pool->FreeList;
            Pool->FreeList = ((pool_block_t *)pMem)->Next; 
            ((pool_block_t*)pMem)->Next = NULL;
            Pool->Level--;
            if (Pool->Level < Pool->LevelMin) {
                Pool->LevelMin = Pool->Level;
            }
        } while (0);
    )

    return pMem;     
}    

pool_uint_t pool_get_level(void *obj)
{
    pool_t *Pool = (pool_t *)obj;

    if (NULL == obj) {
        return 0;
    }

    return Pool->Level;
}

pool_uint_t pool_get_min_level(void *obj)
{
    pool_t *Pool = (pool_t *)obj;

    if (NULL == obj) {
        return 0;
    }

    return Pool->LevelMin;
}


/* EOF */
