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
#include "..\debug\debug.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(queue_t)
    void *              Buffer;
    __queue_uint_t      DataSize;
    __queue_uint_t      Size;
    __queue_uint_t      Head;
    __queue_uint_t      Tail;
    __queue_uint_t      Counter;
    __queue_uint_t      Peek;
    __queue_uint_t      PeekCounter;
#ifdef __QUEUE_MUTEX_TYPE
    __QUEUE_MUTEX_TYPE  Mutex;
#endif
END_DEF_CLASS(queue_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("QUEUE");

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool queue_init(queue_t *QueueObj, void *Buffer, __queue_uint_t Size, __queue_uint_t DataSize)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    
    DEBUG_ASSERT_NOT_NULL(QueueObj);
    DEBUG_ASSERT_NOT_NULL(Buffer);
    DEBUG_ASSERT_EQUAL_UINT(0, Size);
    if (NULL == QueueObj || NULL == Buffer || 0 == Size) {
        return false;
    }

    Queue->Buffer  = Buffer;
    Queue->DataSize = DataSize;
    Queue->Size    = Size;
    Queue->Head    = 0;
    Queue->Tail    = 0;
    Queue->Peek    = 0;
    Queue->Counter = 0;
    Queue->PeekCounter = 0;

    return true;
}

bool queue_deinit(queue_t *QueueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return false;
    }

    Queue->Buffer  = NULL;
    Queue->DataSize = 0;
    Queue->Size    = 0;
    Queue->Head    = 0;
    Queue->Tail    = 0;
    Queue->Peek    = 0;
    Queue->Counter = 0;
    Queue->PeekCounter = 0;

    return true;
}

bool queue_enqueue(queue_t *QueueObj, void *Data)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(Queue->Buffer);
    if (NULL == Queue->Buffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Head ==  Queue->Tail)
            &&  (0 != Queue->Counter)) {
                DEBUG_MSG(QUEUE_DEBUG, "QueueObj is full.");
                break;
            }

            switch (Queue->DataSize) {
                case 1:
                    ((uint8_t *)(Queue->Buffer))[
                                 Queue->Tail] = *(uint8_t *)Data;
                    break;
                case 2:
                    ((uint16_t *)(Queue->Buffer))[
                                  Queue->Tail] = *(uint16_t *)Data;
                    break;
                case 4:
                    ((uint32_t *)(Queue->Buffer))[
                                  Queue->Tail] = *(uint32_t *)Data;
                    break;
                default:
                    mem_copy((uint8_t *)Queue->Buffer + (Queue->DataSize) * (Queue->Tail),
                             Data,
                             Queue->DataSize);
                    break;
            }
            Queue->Tail++;
            if (Queue->Tail >= Queue->Size) {
                Queue->Tail = 0;
            }
            Queue->Counter++;
            Queue->PeekCounter++;
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

bool queue_dequeue(queue_t *QueueObj, void *Data)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(Queue->Buffer);
    if (NULL == Queue->Buffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Head ==  Queue->Tail)
            &&  (0 == Queue->Counter)) {
                DEBUG_MSG(QUEUE_DEBUG, "QueueObj is empty.");
                break;
            }

            if (NULL != Data) {
                switch (Queue->DataSize) {
                    case 1:
                        *(uint8_t *)Data = ((uint8_t *)(Queue->Buffer))[
                                                         Queue->Head];
                        break;
                    case 2:
                        *(uint16_t *)Data = ((uint16_t *)(Queue->Buffer))[
                                                           Queue->Head];
                        break;
                    case 4:
                        *(uint32_t *)Data = ((uint32_t *)(Queue->Buffer))[
                                                           Queue->Head];
                        break;
                    default:
                        mem_copy(Data,
                                 (uint8_t *)Queue->Buffer + (Queue->DataSize) * (Queue->Head),
                                 Queue->DataSize);
                        break;
                }
            }
            Queue->Head++;
            if (Queue->Head >= Queue->Size) {
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

bool queue_peek(queue_t *QueueObj, void *Data)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(Queue->Buffer);
    if (NULL == Queue->Buffer) {
        return false;
    }

    __QUEUE_ATOM_ENTER();
        do {
            if ((Queue->Peek ==  Queue->Tail)
            &&  (0 == Queue->PeekCounter)) {
                DEBUG_MSG(QUEUE_DEBUG, "QueueObj has been peeked all.");
                break;
            }
            if (NULL != Data) {
                switch (Queue->DataSize) {
                    case 1:
                        *(uint8_t *)Data = ((uint8_t *)(Queue->Buffer))[
                                                         Queue->Peek];
                        break;
                    case 2:
                        *(uint16_t *)Data = ((uint16_t *)(Queue->Buffer))[
                                                           Queue->Peek];
                        break;
                    case 4:
                        *(uint32_t *)Data = ((uint32_t *)(Queue->Buffer))[
                                                           Queue->Peek];
                        break;
                    default:
                        *(__queue_uint_t *)Data = ((__queue_uint_t *)(Queue->Buffer))[
                                                           Queue->Peek];
                        break;
                }
            }
            Queue->Peek++;
            Queue->PeekCounter--;
            if (Queue->Peek >= Queue->Size) {
                Queue->Peek = 0;
            }
            bResult = true;
        } while (false);
    __QUEUE_ATOM_EXIT();

    return bResult;
}

void queue_get_all_peeked(queue_t *QueueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return ;
    }

    __QUEUE_ATOM_ENTER();
        Queue->Head = Queue->Peek;
        Queue->Counter = Queue->PeekCounter;
    __QUEUE_ATOM_EXIT();
}

void queue_reset_peek(queue_t *QueueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return ;
    }

    __QUEUE_ATOM_ENTER();
        Queue->Peek = Queue->Head;
        Queue->PeekCounter = Queue->Counter;
    __QUEUE_ATOM_EXIT();
}

__queue_uint_t queue_get_object_count(queue_t *QueueObj)
{
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    __queue_uint_t Counter;

    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj) {
        return 0;
    }

    __QUEUE_ATOM_ENTER();
        Counter = Queue->Counter;
    __QUEUE_ATOM_EXIT();

    return Counter;
}

#ifdef __QUEUE_MUTEX_TYPE
__QUEUE_MUTEX_TYPE *queue_get_mutex(queue_t *QueueObj)
{    
    CLASS(queue_t) *Queue = (CLASS(queue_t) *)QueueObj;
    
    DEBUG_ASSERT_NOT_NULL(QueueObj);
    if (NULL == QueueObj)  {
        return NULL;   
    }

    return &(Queue->Mutex);
}
#endif

