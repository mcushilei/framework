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


#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define POOL_INIT(__POOL, __MEM, __POOL_SIZE, __BLOCK_SIZE)                      \
    pool_init(__POOL, __MEM, __POOL_SIZE, __BLOCK_SIZE)

#define POOL_DEINIT(__POOL) \
    pool_deinit(__POOL)

#define POOL_NEW(__POOL)    \
    pool_new(__POOL)

#define POOL_FREE(__POOL, __PBLOCK)         \
    pool_free(__POOL, __PBLOCK)

#define POOL_GET_ALLOCABLE_COUNT(__POOL)    \
    pool_get_allocable_count(__POOL)

#ifdef __POOL_MUTEX_TYPE
#define POOL_GET_MUTEX(__POOL)  
#endif



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_STRUCTURE(pool_block_t)
    pool_block_t *      Next;
END_DEF_STRUCTURE(pool_block_t)

EXTERN_CLASS(pool_t)
    pool_block_t *  FreeList;
    pool_uint_t     Size;           //!< pool size.
    pool_uint_t     Level;          //!< amount of block allocable
    pool_uint_t     LevelMin;       //!< minimum amount of block allocable
END_EXTERN_CLASS(pool_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool         pool_init       (void *poolObj, void *poolMem, pool_uint_t poolSize, pool_uint_t blockSize);
extern bool         pool_deinit     (void *poolObj);
extern void *       pool_new        (void *poolObj);
extern bool         pool_free       (void *poolObj, void *Block);
extern pool_uint_t  pool_get_level  (void *poolObj);
extern pool_uint_t  pool_get_min_level(void *poolObj);

#endif
/* EOF */
