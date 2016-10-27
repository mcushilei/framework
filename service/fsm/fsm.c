/*******************************************************************************
 *  Copyright(C)2016 by Dreistein<mcu_shilei@hotmail.com>                     *
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




/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm.h"

/*============================ MACROS ========================================*/
//! \brief task pool size
#if ((!defined(FSM_MAX_TASKS)) || (FSM_MAX_TASKS < 1))
#   define FSM_MAX_TASKS           (1u)
#   warning "FSM_MAX_TASKS is invalid, use default value 1u."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name task queue control block
DEF_STRUCTURE(obj_queue_t)
    fsm_obj_t           *ptHead;         //!< queue head
    fsm_obj_t           *ptTail;         //!< queue tail
END_DEF_STRUCTURE(obj_queue_t)

//! \name fsm_scheduler
DEF_STRUCTURE(scheduler_t)
    fsm_tcb_t           *ptCurrentTask;
    task_queue_t        tReadyList;
END_DEF_STRUCTURE(scheduler_t)

/*============================ PROTOTYPES ====================================*/
static bool         fsm_task_stack_push         (
                                                fsm_tcb_t   *pTask,
                                                state_func_t        *fnState,
                                                void                *pArg);
static bool         fsm_task_stack_pop          (fsm_tcb_t *pTask);
static void         fsm_tcb_pool_init           (void);
static void         fsm_tcb_pool_deinit         (void);
static fsm_tcb_t *  fsm_tcb_new                 (
                                                state_func_t        *fnState,
                                                void                *pArg,
                                                task_stack_item_t   *pStack,
                                                uint_fast8_t        chStackSize);
static void         fsm_tcb_free                (fsm_tcb_t *pTask);
static void         fsm_scheduler_init          (void);
static void         fsm_scheduler_deinit        (void);

extern void         fsm_event_init(void);

/*============================ LOCAL VARIABLES ===============================*/
static fsm_tcb_t *  sptTCBFreeList;             //! TCB
static fsm_tcb_t    stTCBs[FSM_MAX_TASKS];      //! TCB pool
static scheduler_t  stScheduler;
static fsm_obj_t *  sptObjRegistedList;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief try to push a routine into stack
 *  \param pTask
 *  \param fnState routine
 *  \param pArg argument for routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
static bool fsm_task_stack_push(
        fsm_tcb_t   *pTask,
        state_func_t        *fnState,
        void                *pArg) 
{
    //! validate parameters.
    if ((NULL == pTask)
    ||  (NULL == pTask->pStack)
    ||  (NULL == fnState)) {
        return false;
    }

    //! if stack full
    if (pTask->chStackLevel >= pTask->chStackSize) {
        return false;
    }

    do {
        task_stack_item_t *ptStack = pTask->pStack;

        ptStack->fnState    = fnState;
        ptStack->pArg       = pArg;
    } while (false);
    pTask->pStack++;
    pTask->chStackLevel++;

    return true;
}

/*! \brief  try to pop a routine from stack
 *  \param  pTask
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
static bool fsm_task_stack_pop(fsm_tcb_t *pTask)
{
    //! validate parameters.
    if ((NULL == pTask)
    ||  (NULL == pTask->pStack)) {
        return false;
    }

    //! if stack empty
    if (0 == pTask->chStackLevel) {
        return false;
    }

    pTask->pStack--;
    pTask->chStackLevel--;

    return true;
}

/*! \brief perform a state transfer.
 *  \param pT a pointer of task control block
 *  \param fnState target state
 *  \param pArg a pointer of argument control block
 *  \retval false invalid param
 *  \retval true succeeded to transfer state
 */
bool fsm_state_transfer(
                state_func_t *      fnState,
                void *              pArg)
{
    fsm_tcb_t *pTask = stScheduler.ptCurrentTask;
    
    if (NULL == pTask) {
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        ptRoutine->fnState  = fnState;
        ptRoutine->pArg     = pArg;
    } while (false);

    return true;
}

/*! \brief call a sub task routine.
 *  \param pTask a pointer of task control block
 *  \param fnState target routine
 *  \param pArg a pointer of argument control block
 *  \param fnReturnRoutine return to this routine when sub FSM completed
 *  \param pReturnArg argument for return routine
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
bool fsm_call_sub_ex(
                state_func_t *      fnState,
                void *              pArg,
                state_func_t *      fnReturnRoutine,
                void *              pReturnArg)
{
    fsm_tcb_t *pTask = stScheduler.ptCurrentTask;

    if ((NULL == pTask)
    ||  (NULL == fnState)
    ||  (NULL == fnReturnRoutine)) {
        return false;
    }

    if (!fsm_task_stack_push(pTask, fnReturnRoutine, pReturnArg)) {
        //!< fatal error! stack is overflow.
        while (fsm_task_stack_pop(pTask));  //!< Terminate this task!
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        ptRoutine->fnState  = fnState;
        ptRoutine->pArg     = pArg;
    } while (false);

    return true;
}

/*! \brief call a sub routine and return current state when sub FSM complete.
 *  \param pT a pointer of task control block
 *  \param fnState target routine
 *  \param pArg a pointer of argument control block
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
bool fsm_call_sub(
                state_func_t *      fnState, 
                void *              pArg)
{
    fsm_tcb_t *pTask = stScheduler.ptCurrentTask;

    if (NULL == pTask) {
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        return fsm_call_sub_ex(
                fnState, pArg, ptRoutine->fnState, ptRoutine->pArg);
    } while (false);
}

/*! \brief get a new task control block from pool and initial it
 *  \param fnState task's first routine
 *  \param pArg a pointer of a argument
 *  \param pStack a return stack buffer
 *  \param chStackSize return stack size
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
static fsm_tcb_t *fsm_tcb_new(
        state_func_t        *fnState,
        void                *pArg,
        task_stack_item_t   *pStack,
        uint_fast8_t        chStackSize)
{
    fsm_tcb_t *pTCB = NULL;

    if ((NULL == fnState)                       //! validate parameters.
    ||  (NULL == pStack) 
    ||  (0 == chStackSize)) {
        return NULL;
    }
    
    if (NULL != sptTCBFreeList) {
        pTCB = sptTCBFreeList;
        sptTCBFreeList = sptTCBFreeList->pNext;
    }

    if (NULL != pTCB) {                         //!< find a task control block ?
        pTCB->pNext            = NULL;

        pTCB->pStack           = pStack;        //!< set stack buffer
        pTCB->pStack->fnState  = fnState;       //!< set task routine    
        pTCB->pStack->pArg     = pArg;          //!< set argument
        pTCB->chStackSize      = chStackSize;   //!< set stack size
        pTCB->chStackLevel     = 0;             //!< set stack point

#if SAFE_TASK_THREAD_SYNC == ENABLED
        pTCB->ptObject         = NULL;
#endif
    }

    return pTCB;
}

/*! \brief  free a task control blcok to pool
 *  \param  pTCB a pointer of a task control block
 *  \return none
 */
static void fsm_tcb_free(fsm_tcb_t *pTCB)
{
    if (NULL == pTCB) {
        return;
    }

    MEM_SET_ZERO((void *)pTCB, sizeof(fsm_tcb_t));

    pTCB->pNext  = sptTCBFreeList;        //! add task item to freelist
    sptTCBFreeList   = pTCB;
}

/*! \brief  create a new task control block
 *  \param  fnState task routine
 *  \param  fnReturnTo a routine which will be called when task routine return
 *          false.
 *  \param  pArg a pointer of a argument control block
 *  \param  pStack a return stack buffer
 *  \param  chStackSize return stack size
 *  \return task handle
 */
uint_fast8_t fsm_task_create(
        fsm_tcb_t **        pptTask,
        state_func_t *      fnState,
        void *              pArg,
        task_stack_item_t * pStack,
        uint_fast8_t        chStackSize)
{
    fsm_tcb_t *ptTask;
    
    if (NULL == fnState) {
        return FSM_ERR_NULL_PTR;
    }
    
    /*! try to get a TCB */
    ptTask = fsm_tcb_new(fnState, pArg, pStack, chStackSize);
    if (NULL == ptTask) {
        return FSM_ERR_TASK_NO_MORE_TCB;
    }
    
    if (fsm_set_task_ready(ptTask)) {
        if (pptTask != NULL) {
            *pptTask = ptTask;
        }
        return FSM_ERR_NONE;
    } else {
        fsm_tcb_free(ptTask);
        return FSM_ERR_TASK_FULL;
    }
}

/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param pTask a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
static bool fsm_task_enqueue(task_queue_t *pTaskQueue, fsm_tcb_t *pTask)
{
    pTask->pNext = NULL;
    SAFE_ATOM_CODE(
        if (NULL == pTaskQueue->ptTCBTail) {        //!< is this queue empty?
            pTaskQueue->ptTCBHead = pTask;          //!< update task head
        } else {                                //!< queue is not empty
            pTaskQueue->ptTCBTail->pNext = pTask;   //!< add a new task to tail
        }
        pTaskQueue->ptTCBTail = pTask;              //!< move tail
    )

    return true;
}

/*! \brief  remove a task control block from a specified task control queue
 *  \param  pTaskQueue a pointer of a task queue control block
 *  \param  pTask a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded to get a task from queue
 */
static fsm_tcb_t *fsm_task_dequeue(task_queue_t *pTaskQueue)
{
    fsm_tcb_t *pTask = NULL;

    SAFE_ATOM_CODE(
        pTask = pTaskQueue->ptTCBHead;
        if (NULL != pTask) {                    //!< is this queue empty?
            pTaskQueue->ptTCBHead = pTask->pNext;   //!< update head

            if (NULL == pTaskQueue->ptTCBHead) {    //!< queue is empty
                pTaskQueue->ptTCBTail = NULL;       //!< update tail
            }
        }
    )

    return pTask;
}

static bool fsm_remove_task_from_queue(task_queue_t *pTaskQueue, fsm_tcb_t *pTask)
{
    fsm_tcb_t **ppTCB;
    
    for (ppTCB = &pTaskQueue->ptTCBHead; 
         *ppTCB;
         ppTCB = &((*ppTCB)->pNext)) {
        if (*ppTCB == pTask) {
            if (pTask == pTaskQueue->ptTCBHead) {
                if (pTask == pTaskQueue->ptTCBTail) {
                    pTaskQueue->ptTCBTail = NULL;
                }
                pTaskQueue->ptTCBHead = pTask->pNext;
            } else if (pTask == pTaskQueue->ptTCBTail) {
                *ppTCB = NULL;
                for (pTask = pTaskQueue->ptTCBHead; pTask->pNext != NULL; pTask = pTask->pNext);
                pTaskQueue->ptTCBTail = pTask;
            }
            return true;
        }
    }
    
    return false;
}

/*! \brief  add a task to ready table(list queue).
 *  \param  pTask task control block
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
bool fsm_set_task_ready(fsm_tcb_t *pTask)
{
    fsm_task_enqueue(&stScheduler.tReadyList, pTask);  //!< just enqueue.

    return true;
}

/*! \brief  fsm_scheduler function
 *  \retval true system is busy
 *  \retval false system is idle, no task to run.
 */
bool fsm_scheduler(void)
{  
    fsm_tcb_t *pTask                = NULL;
    task_stack_item_t *ptRoutine    = NULL;

    /* get a task from queue */
    pTask = fsm_task_dequeue(&stScheduler.tReadyList);
    if (NULL == pTask) {
        return false;
    }

    stScheduler.ptCurrentTask = pTask;
    ptRoutine = pTask->pStack;

    /* run task routine */
    ptRoutine->fnState(ptRoutine->pArg);

    do {
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        /* Is task waitting synchronization object to set? */
        if (NULL != pTask->ptObject) {           //!< task is waitting synchronization object
            break;
        }
    #endif
        
        /* check whether this fsm is complete. */
        if (ptRoutine->fnState != NULL) {       
            fsm_set_task_ready(pTask);     //!< re-add this task to queue
            break;
        } else {                                //!< it has returned from a sub-fsm.
            if (fsm_task_stack_pop(pTask)) {        //!< has parent fsm?
                fsm_set_task_ready(pTask); //!< re-add this task to queue
                break;
            }
        }
        
        fsm_tcb_free(pTask);            //!< this task finished, free task
    } while (false);

    return true;
}

bool fsm_register_object(void *pObj)
{
    fsm_obj_t *ptObject = pObj;
    
    ptObject->ptObjNext = sptObjRegistedList;
    sptObjRegistedList = ptObject;

    return true;
}

bool fsm_deregister_object(void *pObj)
{
    fsm_obj_t **pptObject;
    fsm_obj_t *ptObject = pObj;

    for (pptObject = &sptObjRegistedList; *pptObject != NULL; pptObject = &((*pptObject)->ptObjNext)) {
        if (*pptObject == ptObject) {
            *pptObject = (*pptObject)->ptObjNext;
            ptObject->ptObjNext = NULL;
            return true;
        }
    }

    return false;
}

void fsm_time_tick(void)
{
    fsm_obj_t *ptObject;

    for (ptObject = sptObjRegistedList; ptObject != NULL; ptObject = ptObject->ptObjNext) {
        if (ptObject->chObjType & 0x80) {   //!< only watiable object can be wait.
            fsm_waitable_obj_header_t *ptWatiableObj = (fsm_waitable_obj_header_t *)ptObject;
            if (ptWatiableObj->ptTCBHead) { //!< if there is task wait for this obj.
                fsm_tcb_t *pTCBs;
                for (pTCBs = ptWatiableObj->ptTCBHead; pTCBs; pTCBs = pTCBs->pNext) {
                    if (pTCBs->hwDelay != 0) {
                        pTCBs->hwDelay--;
                        if (pTCBs->hwDelay == 0) {
                            //! 1. remove this task from this obj's wait queue.
                            //! 2. set tcb.ptObject NULL
                            //! 3. set tcb.chStatus for reasion OBJ_WAIT_TIMEOUT.
                        }
                    }
                }
            }
        }
    }
}

#if SAFE_TASK_THREAD_SYNC == ENABLED
/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  pTask parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
uint_fast8_t fsm_wait_for_single_object(void *ptObject)
{
    uint_fast8_t    bResult;
    uint8_t         chObjType;
    fsm_tcb_t *     pTask = stScheduler.ptCurrentTask;

    if (NULL != pTask->ptObject) {   //!< fatal error!
        return FSM_ERR_TASK_ONLY_WAIT_SINGLE_OBJ;
    }
    
    if (NULL == ptObject) {          //!< fatal error!
        return FSM_ERR_NULL_PTR;
    }
    
    chObjType = ((fsm_obj_t *)ptObject)->chObjType;
    if (!(chObjType & 0x80)) {
        return FSM_ERR_OBJ_NOT_WAITABLE;
    }
    
    bResult = FSM_ERR_OBJ_NOT_SINGLED;
    switch (chObjType) {
    case FSM_OBJ_TYPE_EVENT:
        SAFE_ATOM_CODE(
            do {
                fsm_event_t *ptEvent = (fsm_event_t *)ptObject;
                bResult = ptEvent->chSignal;
                if (ptEvent->chSignal & FSM_SIGNAL_FLAG_BIT) {                 //!< obj's signal raised.
                    bResult = FSM_ERR_NONE;
                    if (!(ptEvent->chSignal & FSM_SIGNAL_MANUAL_RESET_BIT)) {
                        ptEvent->chSignal &= ~FSM_SIGNAL_FLAG_BIT;             //!< clear obj's signal.
                    }
                } else {                            //!< obj's signal not raised.
                    //! add task to the object's wait queue.
                    pTask->pNext = NULL;
                    if (NULL == ptEvent->ptTCBTail) {
                        ptEvent->ptTCBHead = pTask;
                    } else {
                        ptEvent->ptTCBTail->pNext = pTask;
                    }
                    ptEvent->ptTCBTail = pTask;

                    pTask->ptObject = (fsm_obj_t *)ptObject;
                }
            } while (0);
        )
        break;
        
    case FSM_OBJ_TYPE_INVALID:
    default:
        break;
    }

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

/*! \brief  add task control blocks to the task pool
 *  \param  pHead task heap buffer
 *  \param  hwSize heap size
 *  \return access result
 */
static void fsm_tcb_pool_init(void)
{
    uint_fast8_t n = 0;
    fsm_tcb_t *p = stTCBs;

    MEM_SET_ZERO((void *)stTCBs, sizeof(stTCBs));

    //! add tasks to the free list
    for (n = UBOUND(stTCBs); n; n--) {
        p->pNext    = sptTCBFreeList;    //! add task item to freelist
        sptTCBFreeList  = p;
        p++;
    }
}

/*! \brief  
 *  \param  none
 *  \return access result
 */
static void fsm_tcb_pool_deinit(void)
{
    sptTCBFreeList = NULL;
    MEM_SET_ZERO((void *)stTCBs, sizeof(stTCBs));
}

/*! \brief initialize fsm_scheduler
 *  \param none
 *  \return none
 */
static void fsm_scheduler_init(void)
{
    MEM_SET_ZERO((void *)&stScheduler, sizeof(stScheduler));
}

/*! \brief deinitialize fsm_scheduler
 *  \param none
 *  \return none
 */
static void fsm_scheduler_deinit(void)
{
    MEM_SET_ZERO((void *)&stScheduler, sizeof(stScheduler));
}

/*! \brief fsm init.
 */
void fsm_init(void)
{
    fsm_tcb_pool_init();
    fsm_event_init();
    fsm_scheduler_init();
}

/*! \brief fsm init.
 */
void fsm_deinit(void)
{
    fsm_scheduler_deinit();
    fsm_tcb_pool_deinit();
}


/* EOF */
