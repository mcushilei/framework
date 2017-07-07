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


#ifndef __POOL_C__
#ifndef __POOL_H__
#define __POOL_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_STRUCTURE(pool_block_t)
    pool_block_t*   pNext;
END_DEF_STRUCTURE(pool_block_t)

EXTERN_CLASS(pool_t)
    pool_block_t*   pFreeList;
    pool_uint_t     Size;           //!< pool size.
    pool_uint_t     Level;          //!< amount of block allocable
    pool_uint_t     LevelMin;       //!< minimum amount of block allocable
END_EXTERN_CLASS(pool_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool         pool_init       (void* pPoolObj, pool_uint_t blockSize);
extern bool         pool_deinit     (void* pPoolObj);
extern bool         pool_add_memory (void* pPoolObj, void* pMem, pool_uint_t num);
extern void*        pool_new        (void* pPoolObj);
extern bool         pool_free       (void* pPoolObj, void* pMem);
extern pool_uint_t  pool_get_level  (void* pPoolObj);
extern pool_uint_t  pool_get_min_level(void* pPoolObj);

#endif      //!< #ifndef __POOL_H__
#endif      //!< #ifndef __POOL_C__
/* EOF */
