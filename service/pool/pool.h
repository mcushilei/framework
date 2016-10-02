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
EXTERN_CLASS(pool_t)
    void *              ptFreeList;
    __pool_uint_t       tBlockSize;
    __pool_uint_t       tSize;
    __pool_uint_t       tCounter;       //!< amount of block allocable
#ifdef __POOL_MUTEX_TYPE
    __POOL_MUTEX_TYPE   tMutex;
#endif
END_EXTERN_CLASS(pool_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool             pool_init(pool_t *      pPOOL,
                                  void *        pMem,
                                  __pool_uint_t tPoolSize,
                                  __pool_uint_t tBlockSize);
extern bool             pool_deinit(pool_t *pPOOL);
extern void *           pool_new(pool_t *pPOOL);
extern bool             pool_free(pool_t *pPOOL, void *pBLOCK);
extern __pool_uint_t    pool_get_allocable_count(pool_t *pPOOL);
#ifdef __POOL_MUTEX_TYPE
extern __POOL_MUTEX_TYPE *pool_get_mutex(pool_t *pPOOL);
#endif

#endif
/* EOF */
