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


#ifndef __QUEUE_H__
#define __QUEUE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define QUEUE_INIT(__PQUEUE, __PBUF, __BUF_SIZE, __OBJ_SIZE)              \
    queue_init(__PQUEUE, __PBUF, __BUF_SIZE, __OBJ_SIZE)

#define QUEUE_UINT8_INIT(__PQUEUE, __PBUF, __BUF_SIZE)                    \
    QUEUE_INIT(__PQUEUE, __PBUF, __BUF_SIZE, sizeof(uint8_t))

#define QUEUE_UINT16_INIT(__PQUEUE, __PBUF, __BUF_SIZE)                   \
    QUEUE_INIT(__PQUEUE, __PBUF, __BUF_SIZE, sizeof(uint16_t))

#define QUEUE_UINT32_INIT(__PQUEUE, __PBUF, __BUF_SIZE)                   \
    QUEUE_INIT(__PQUEUE, __PBUF, __BUF_SIZE, sizeof(uint32_t))

#define QUEUE_DEINIT(__PQUEUE)              queue_deinit(__PQUEUE)
#define QUEUE_ENQUEUE(__PQUEUE, __OBJ)      queue_enqueue(__PQUEUE, &(__OBJ))
#define QUEUE_DEQUEUE(__PQUEUE, __POBJ)     queue_dequeue(__PQUEUE, __POBJ)
#define QUEUE_PEEK(__PQUEUE, __POBJ)        queue_peek(__PQUEUE, __POBJ)
#define QUEUE_GET_ALL_PEEKED(__PQUEUE)      queue_get_all_peeked(__PQUEUE)
#define QUEUE_RESET_PEEK(__PQUEUE)          queue_reset_peek(__PQUEUE)
#define QUEUE_GET_OBJECT_COUNT(__PQUEUE)    queue_get_object_count(__PQUEUE)

/*============================ TYPES =========================================*/
EXTERN_CLASS(queue_t)
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
END_EXTERN_CLASS(queue_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool queue_init(queue_t *pQUEUE, void *pBuffer, __queue_uint_t tSize, __queue_uint_t tObjSize);
extern bool queue_deinit(queue_t *pQUEUE);
extern bool queue_enqueue(queue_t *pQUEUE, void *ptObj);
extern bool queue_dequeue(queue_t *pQUEUE, void *ptObj);
extern bool queue_peek(queue_t *pQUEUE, void *ptObj);
extern void queue_get_all_peeked(queue_t *pQUEUE);
extern void queue_reset_peek(queue_t *pQUEUE);
extern __queue_uint_t queue_get_object_count(queue_t *pQUEUE);
#ifdef __QUEUE_MUTEX_TYPE
extern __QUEUE_MUTEX_TYPE *queue_get_mutex(queue_t *pQUEUE);
#endif

#endif
