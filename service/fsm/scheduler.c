/***************************************************************************
 *   Copyright(C)2009-2010 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//! \brief this is a cooperation sheduler, a task is a FSM

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm_types.h"
#include ".\task.h"

/*============================ MACROS ========================================*/
//! \brief task queue pool size. More large this value is lead to more random
//!        the program runs but takes more time to scedule task.
#if ((!defined(TASK_QUEUE_POOL_SIZE)) || (TASK_QUEUE_POOL_SIZE < 1) || (TASK_QUEUE_POOL_SIZE > 16))
#   define TASK_QUEUE_POOL_SIZE         (1u)
#   warning "TASK_QUEUE_POOL_SIZE is invalid, use default value 1u."
#endif

//! \brief task pool size
#if ((!defined(TASK_TCB_POOL_SIZE)) || (TASK_TCB_POOL_SIZE < 1))
#   define TASK_TCB_POOL_SIZE           (1u)
#   warning "TASK_TCB_POOL_SIZE is invalid, use default value 1u."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name task queue control block
DEF_STRUCTURE(task_queue_t)
    task_ctrl_block_t   *pHead;         //!< queue head
    task_ctrl_block_t   *pTail;         //!< queue tail
#if TASK_QUEUE_POOL_SIZE > 1
    task_queue_t        *ptNext;
    uint8_t             chID;
#endif
END_DEF_STRUCTURE(task_queue_t)

//! \name scheduler
DEF_STRUCTURE(scheduler_t)
    task_ctrl_block_t   *ptCurrentTask;
#if TASK_QUEUE_POOL_SIZE > 1
    //! \brief free task queue list
    task_queue_t        *ptQHead;
    task_queue_t        *ptQTail;
    uint16_t            hwQueueBusyFlag;
#endif
END_DEF_STRUCTURE(scheduler_t)

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
scheduler_t g_tScheduler;

/*============================ LOCAL VARIABLES ===============================*/
//! \brief task queue pool
static task_queue_t s_TaskQueuePool[TASK_QUEUE_POOL_SIZE];

/*============================ IMPLEMENTATION ================================*/
#if TASK_QUEUE_POOL_SIZE > 1
#define RELEASE_TASK_QUEUE(__PQ) do {                   \
            __PQ->ptNext = NULL;                        \
            if (NULL == g_tScheduler.ptQTail) {         \
                g_tScheduler.ptQHead = __PQ;            \
            } else {                                    \
                g_tScheduler.ptQTail->ptNext = __PQ;    \
            }                                           \
            g_tScheduler.ptQTail = __PQ;                \
        } while (false)

static void release_task_queue(task_queue_t *ptQueue)
{
    RELEASE_TASK_QUEUE(ptQueue);
}

static task_queue_t *get_task_queue(void)
{
    task_queue_t *ptQueue = NULL;

    ptQueue = g_tScheduler.ptQHead;
    if (NULL != ptQueue) {
        g_tScheduler.ptQHead = ptQueue->ptNext;
        if (NULL == g_tScheduler.ptQHead) {
            g_tScheduler.ptQTail = g_tScheduler.ptQHead;
        }
        ptQueue->ptNext = NULL;
    }

    return ptQueue;
}

static void init_task_queue(void)
{
    uint_fast16_t n = 0;
    task_queue_t *ptQueue = s_TaskQueuePool;
    
    for (n = UBOUND(s_TaskQueuePool); n; --n) {
        ptQueue->chID = n - 1;
        RELEASE_TASK_QUEUE(ptQueue);
        ptQueue++;
    }
}
#endif      //! #if TASK_QUEUE_POOL_SIZE > 1

/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param pTask a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
static bool add_task_to_queue(task_queue_t *pTaskQueue, task_ctrl_block_t *pTask)
{
    pTask->pNext = NULL;
    SAFE_ATOM_CODE(
        if (NULL == pTaskQueue->pTail) {        //!< is this queue empty?
            pTaskQueue->pHead = pTask;          //!< update task head
#if TASK_QUEUE_POOL_SIZE > 1
            g_tScheduler.hwQueueBusyFlag |= BIT(pTaskQueue->chID);  //!< set flag to indicate this queue has task to run.
#endif
        } else {                                //!< queue is not empty
            pTaskQueue->pTail->pNext = pTask;   //!< add a new task to tail
        }
        pTaskQueue->pTail = pTask;              //!< move tail
    )

    return true;
}

/*! \brief  get a task control block from a specified task control queue
 *  \param  pTaskQueue a pointer of a task queue control block
 *  \param  pTask a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded to get a task from queue
 */
static task_ctrl_block_t *remove_task_from_queue(task_queue_t *pTaskQueue)
{
    task_ctrl_block_t *pTask = NULL;

    SAFE_ATOM_CODE(
        pTask = pTaskQueue->pHead;
        if (NULL != pTask) {                    //!< is this queue empty?
            pTaskQueue->pHead = pTask->pNext;   //!< update head

            if (NULL == pTaskQueue->pHead) {    //!< queue is empty
                pTaskQueue->pTail = NULL;       //!< update tail
#if TASK_QUEUE_POOL_SIZE > 1
                g_tScheduler.hwQueueBusyFlag &= ~BIT(pTaskQueue->chID);    //!< clear flag
#endif
            }
        }
    )

    return pTask;
}

/*! \brief  add a task to ready table(list queue).
 *  \param  pTask task control block
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
bool scheduler_register_task(task_ctrl_block_t *pTask)
{
#if TASK_QUEUE_POOL_SIZE > 1
    task_queue_t *ptTaskQueue = NULL;

    SAFE_ATOM_CODE(
        ptTaskQueue = get_task_queue();             //!< get a queue.
        if (NULL != ptTaskQueue) {
            add_task_to_queue(ptTaskQueue, pTask);  //!< add task to this queue.
            release_task_queue(ptTaskQueue);        //!< release this queue to list.
        } else {
            //! fatal error.
        }
    )
#else
    add_task_to_queue(&s_TaskQueuePool[0], pTask);  //!< just enqueue.
#endif

    return true;
}

/*! \brief  scheduler function
 *  \retval true system is busy
 *  \retval false system is idle, no task to run.
 */
bool scheduler(void)
{  
#if TASK_QUEUE_POOL_SIZE > 1
    task_queue_t *ptQueue = NULL;
#endif
    task_ctrl_block_t *pTask        = NULL;
    task_stack_item_t *ptRoutine    = NULL;

    /* get a task from queue */
#if TASK_QUEUE_POOL_SIZE > 1
    SAFE_ATOM_CODE(
        do {
            ptQueue = get_task_queue();
            if (NULL == ptQueue) {
                break;
            }
            pTask = remove_task_from_queue(ptQueue);
            release_task_queue(ptQueue);
        } while (false);
    )
    if (NULL == pTask) {
        return g_tScheduler.hwQueueBusyFlag ? true : false;
    }
#else       //! #if TASK_QUEUE_POOL_SIZE > 1
    pTask = remove_task_from_queue(s_TaskQueuePool);
    if (NULL == pTask) {
        return false;
    }
#endif

    g_tScheduler.ptCurrentTask = pTask;
    ptRoutine = pTask->pStack;

    /* run task routine */
    ptRoutine->fnState(ptRoutine->pArg);

    do {
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        /* Is task waitting synchronization object to set? */
        if (NULL != pTask->pObject) {           //!< task is waitting synchronization object
            break;
        }
    #endif
        
        /* check whether this fsm is complete. */
        if (ptRoutine->fnState != NULL) {       
            scheduler_register_task(pTask);     //!< re-add this task to queue
            break;
        } else {                                //!< it has returned from a sub-fsm.
            if (task_stack_pop(pTask)) {        //!< has parent fsm?
                scheduler_register_task(pTask); //!< re-add this task to queue
                break;
            }
        }
        
        task_ctrl_block_free(pTask);            //!< this task finished, free task
    } while (false);

#if TASK_QUEUE_POOL_SIZE > 1
    return g_tScheduler.hwQueueBusyFlag ? true : false;
#else       //! #if TASK_QUEUE_POOL_SIZE > 1
    return true;
#endif
}

#if SAFE_TASK_THREAD_SYNC == ENABLED
/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  pTask parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
bool scheduler_wait_for_single_object(event_t *pObject)
{
    bool bResult;
    task_ctrl_block_t *pTask = g_tScheduler.ptCurrentTask;

    if (NULL == pTask) {            //!< fatal error!
        return false;
    }
    
    if (NULL != pTask->pObject) {   //!< fatal error!
        return false;
    }
    
    if (NULL == pObject) {          //!< fatal error!
        return true;
    }
    
    SAFE_ATOM_CODE(
        bResult = pObject->bSignal;
        if (bResult) {                      //!< obj's signal raised.
            if (!pObject->bManualReset) {
                pObject->bSignal = false;   //!< clear obj's signal.
            }
        } else {                            //!< obj's signal not raised.
            //! add task to the wait list of object
            pTask->pNext = NULL;
            if (NULL == pObject->ptTail) {
                pObject->ptHead = pTask;
            } else {
                pObject->ptTail->pNext = pTask;
            }
            pObject->ptTail = pTask;

            pTask->pObject = pObject;
        }
    )

    return bResult;
}

/*! \brief Demo
event_t tEvent;

STATE(Demo) BEGIN
    if (!wait_for_single_object(&tEvent)) {
        WAIT_FOR_OBJ;
    }
    ...
END
 */
#endif

/*! \brief initialize scheduler
 *  \param none
 *  \return none
 */
void scheduler_init(void)
{
    MEM_SET_ZERO((void *)s_TaskQueuePool, sizeof(s_TaskQueuePool));

#if TASK_QUEUE_POOL_SIZE > 1
    init_task_queue();
#endif
}

/*! \brief deinitialize scheduler
 *  \param none
 *  \return none
 */
void scheduler_deinit(void)
{
    MEM_SET_ZERO((void *)s_TaskQueuePool, sizeof(s_TaskQueuePool));
}

/* EOF */
