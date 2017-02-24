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
    pool_block_t *  Next;
END_DEF_STRUCTURE(pool_block_t)

//! ADT: list stack.
DEF_CLASS(pool_t)
    pool_block_t *  FreeList;
    pool_uint_t     Size;           //!< pool size.
    pool_uint_t     Level;          //!< amount of block allocable
    pool_uint_t     LevelMin;       //!< minimum amount of block allocable
END_DEF_CLASS(pool_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool pool_init(void *poolObj, void *poolMem, pool_uint_t poolSize, pool_uint_t blockSize)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;
    pool_block_t **ppBlockRef;

    if ((NULL == poolObj) || (NULL == poolMem) || (0 == poolSize) || (sizeof(void *) > blockSize)) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = poolSize;
    Pool->Level     = poolSize;
    Pool->LevelMin  = poolSize;

    ppBlockRef = &Pool->FreeList;
    for (; poolSize; poolSize--) {
        *ppBlockRef = poolMem;
        ppBlockRef  = &((pool_block_t *)(poolMem))->Next;
        *ppBlockRef = NULL;
        poolMem = (uint8_t *)poolMem + blockSize;
    }

    return true;
}

bool pool_deinit(void *poolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;

    if (NULL == poolObj) {
        return false;
    }

    Pool->FreeList  = NULL;
    Pool->Size      = 0;
    Pool->Level     = 0;
    Pool->LevelMin  = 0;

    return true;
}

bool pool_free(void *poolObj, void *Block)
{    
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;

    if ((NULL == poolObj) || (NULL == Block)) {
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

void *pool_new(void *poolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;
    void *Block = NULL;

    if (NULL == poolObj) {
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

pool_uint_t pool_get_level(void *poolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;

    if (NULL == poolObj) {
        return 0;
    }

    return Pool->Level;
}

pool_uint_t pool_get_min_level(void *poolObj)
{
    CLASS(pool_t) *Pool = (CLASS(pool_t) *)poolObj;

    if (NULL == poolObj) {
        return 0;
    }

    return Pool->LevelMin;
}


/* EOF */
