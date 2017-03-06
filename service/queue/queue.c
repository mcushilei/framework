/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify it   *
 *  under the terms of the GNU Lesser General Public License as published     *
 *  by the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful, but       *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  General Public License for more details.                                  *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with Queue program; if not, write to the Free Software Foundation,   *
 *  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
*******************************************************************************/

//! \note do not move Queue pre-processor statement to other places
#define __QUEUE_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\string\string.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(queue_t)
    void *              pBuffer;
    __queue_uint_t      bufferSize;
    __queue_uint_t      itemSize;
    __queue_uint_t      Head;
    __queue_uint_t      Tail;
    __queue_uint_t      Counter;
    __queue_uint_t      Peek;
    __queue_uint_t      PeekCounter;
END_DEF_CLASS(queue_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool queue_init(void *queueObj, void *pBuffer, __queue_uint_t bufferSize, __queue_uint_t itemSize)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;
    
    if (NULL == Queue || NULL == pBuffer || 0 == bufferSize) {
        return false;
    }

    Queue->pBuffer  = pBuffer;
    Queue->itemSize = itemSize;
    Queue->bufferSize   = bufferSize;
    Queue->Head     = 0;
    Queue->Tail     = 0;
    Queue->Peek     = 0;
    Queue->Counter  = 0;
    Queue->PeekCounter = 0;

    return true;
}

bool queue_deinit(void *queueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;

    if (NULL == Queue) {
        return false;
    }

    Queue->pBuffer  = NULL;
    Queue->itemSize = 0;
    Queue->bufferSize   = 0;
    Queue->Head     = 0;
    Queue->Tail     = 0;
    Queue->Peek     = 0;
    Queue->Counter  = 0;
    Queue->PeekCounter = 0;

    return true;
}

bool queue_enqueue(void *queueObj, void *pData)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;
    bool bResult = false;

    if (NULL == Queue) {
        return false;
    }

    if (NULL == Queue->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Head ==  Queue->Tail)
            &&  (0 != Queue->Counter)) {
                break;
            }

            switch (Queue->itemSize) {
                case sizeof(uint8_t):
                    ((uint8_t *)(Queue->pBuffer))[
                                 Queue->Tail] = *(uint8_t *)pData;
                    break;
                case sizeof(uint16_t):
                    ((uint16_t *)(Queue->pBuffer))[
                                  Queue->Tail] = *(uint16_t *)pData;
                    break;
                case sizeof(uint32_t):
                    ((uint32_t *)(Queue->pBuffer))[
                                  Queue->Tail] = *(uint32_t *)pData;
                    break;
                default:
                    mem_copy((void *)((uint32_t)Queue->pBuffer + Queue->Tail * Queue->itemSize),
                             pData,
                             Queue->itemSize);
                    break;
            }
            Queue->Tail++;
            if (Queue->Tail >= Queue->bufferSize) {
                Queue->Tail = 0;
            }
            Queue->Counter++;
            Queue->PeekCounter++;
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

bool queue_dequeue(void *queueObj, void *pData)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;
    bool bResult = false;

    if (NULL == Queue) {
        return false;
    }

    if (NULL == Queue->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Head ==  Queue->Tail)
            &&  (0 == Queue->Counter)) {
                break;
            }

            if (NULL != pData) {
                switch (Queue->itemSize) {
                    case sizeof(uint8_t):
                        *(uint8_t *)pData = ((uint8_t *)(Queue->pBuffer))[
                                                         Queue->Head];
                        break;
                    case sizeof(uint16_t):
                        *(uint16_t *)pData = ((uint16_t *)(Queue->pBuffer))[
                                                           Queue->Head];
                        break;
                    case sizeof(uint32_t):
                        *(uint32_t *)pData = ((uint32_t *)(Queue->pBuffer))[
                                                           Queue->Head];
                        break;
                    default:
                        mem_copy(pData,
                                 (void *)((uint32_t)Queue->pBuffer + Queue->Head * Queue->itemSize),
                                 Queue->itemSize);
                        break;
                }
            }
            Queue->Head++;
            if (Queue->Head >= Queue->bufferSize) {
                Queue->Head = 0;
            }
            Queue->Counter--;
            Queue->Peek = Queue->Head;
            Queue->PeekCounter = Queue->Counter;
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

bool queue_peek(void *queueObj, void *pData)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;
    bool bResult = false;

    if (NULL == Queue) {
        return false;
    }

    if (NULL == Queue->pBuffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Peek == Queue->Tail)
            &&  (0 == Queue->PeekCounter)) {
                break;
            }
            if (NULL != pData) {
                switch (Queue->itemSize) {
                    case sizeof(uint8_t):
                        *(uint8_t *)pData = ((uint8_t *)(Queue->pBuffer))[
                                                         Queue->Peek];
                        break;
                    case sizeof(uint16_t):
                        *(uint16_t *)pData = ((uint16_t *)(Queue->pBuffer))[
                                                           Queue->Peek];
                        break;
                    case sizeof(uint32_t):
                        *(uint32_t *)pData = ((uint32_t *)(Queue->pBuffer))[
                                                           Queue->Peek];
                        break;
                    default:
                        mem_copy(pData,
                                 (void *)((uint32_t)Queue->pBuffer + Queue->Peek * Queue->itemSize),
                                 Queue->itemSize);
                        break;
                }
            }
            Queue->Peek++;
            Queue->PeekCounter--;
            if (Queue->Peek >= Queue->bufferSize) {
                Queue->Peek = 0;
            }
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

void queue_get_all_peeked(void *queueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;

    if (NULL == Queue) {
        return ;
    }

    __QUEUE_ATOM_ENTER();
        Queue->Head = Queue->Peek;
        Queue->Counter = Queue->PeekCounter;
    __QUEUE_ATOM_EXIT();
}

void queue_reset_peek(void *queueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;

    if (NULL == Queue) {
        return;
    }

    __QUEUE_ATOM_ENTER();
        Queue->Peek = Queue->Head;
        Queue->PeekCounter = Queue->Counter;
    __QUEUE_ATOM_EXIT();
}

__queue_uint_t queue_get_object_count(void *queueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)queueObj;
    __queue_uint_t Counter;

    if (NULL == Queue) {
        return 0;
    }

    __QUEUE_ATOM_ENTER();
        Counter = Queue->Counter;
    __QUEUE_ATOM_EXIT();

    return Counter;
}

