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
//! \name fsm scheduler
DEF_STRUCTURE(scheduler_t)
    fsm_tcb_t           *ptCurrentTask;
    task_queue_t        tReadyList;
END_DEF_STRUCTURE(scheduler_t)

/*============================ PROTOTYPES ====================================*/
static bool         fsm_task_stack_push         (
                                                fsm_tcb_t *         Task,
                                                state_func_t *      State,
                                                void *              Arg);
static bool         fsm_task_stack_pop          (fsm_tcb_t *        Task);
static void         fsm_tcb_pool_init           (void);
static fsm_tcb_t *  fsm_tcb_new                 (
                                                state_func_t *      State,
                                                void *              Arg,
                                                task_stack_t * Stack,
                                                uint_fast8_t        StackSize);
static void         fsm_tcb_free                (fsm_tcb_t *        Task);

extern void         fsm_event_init              (void);
extern void         fsm_mutex_init              (void);
extern void         fsm_semaphore_init          (void);

/*============================ LOCAL VARIABLES ===============================*/
static fsm_tcb_t *  sptTCBFreeList;             //! TCB
static fsm_tcb_t    stTCBs[FSM_MAX_TASKS];      //! TCB pool
static scheduler_t  stScheduler;
static fsm_basis_obj_t *  sObjRegistedList;
volatile uint8_t    gchFSMIntNesting;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief try to push a routine into stack
 *  \param Task
 *  \param State routine
 *  \param Arg argument for routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
static bool fsm_task_stack_push(
        fsm_tcb_t *          Task,
        state_func_t *       State,
        void *               Arg)
{
    task_stack_t *ptStack;
    
    //! if stack full
    if ((Task->StackPoint + 1) >= Task->StackSize) {
        return false;
    }

    Task->StackPoint++;
    ptStack = Task->Stack + Task->StackPoint;
    ptStack->State = State;
    ptStack->Arg    = Arg;

    return true;
}

/*! \brief  try to pop a routine from stack
 *  \param  Task
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
static bool fsm_task_stack_pop(fsm_tcb_t *Task)
{
    //! if stack empty
    if (0 == Task->StackPoint) {
        return false;
    }

    Task->StackPoint--;

    return true;
}

/*! \brief perform a state transfer.
 *  \param pT a pointer of task control block
 *  \param State target state
 *  \param Arg a pointer of argument control block
 *  \retval false invalid param
 *  \retval true succeeded to transfer state
 */
bool fsm_state_transfer(
                state_func_t *      State,
                void *              Arg)
{
    fsm_tcb_t *         Task     = stScheduler.ptCurrentTask;
    task_stack_t * ptRoutine = Task->Stack + Task->StackPoint;

    ptRoutine->State  = State;
    ptRoutine->Arg     = Arg;

    return true;
}

/*! \brief call a sub task routine.
 *  \param State target routine
 *  \param Arg a pointer of argument
 *  \param ReturnState return to this routine when sub FSM completed
 *  \param ReturnArg argument for return routine
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
uint_fast8_t fsm_call_sub_ex(
        state_func_t *      State,
        void *              Arg,
        state_func_t *      ReturnState,
        void *              ReturnArg)
{
    fsm_tcb_t *Task = stScheduler.ptCurrentTask;

    if ((NULL == State)
    ||  (NULL == ReturnState)) {
        return FSM_ERR_INVALID_PARAM;
    }

    do {
        task_stack_t *ptRoutine = Task->Stack + Task->StackPoint;

        ptRoutine->State  = ReturnState;
        ptRoutine->Arg     = ReturnArg;
    } while (0);

    if (!fsm_task_stack_push(Task, State, Arg)) {
        return FSM_ERR_TASK_STACK_FULL;
    }

    return FSM_ERR_NONE;
}

/*! \brief call a sub routine and return CURRENT state when sub fsm complete.
 *  \param pT a pointer of task control block
 *  \param State target routine
 *  \param Arg a pointer of argument control block
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
uint_fast8_t fsm_call_sub(
        state_func_t *      State, 
        void *              Arg)
{
    fsm_tcb_t *Task = stScheduler.ptCurrentTask;
    task_stack_t *ptRoutine = Task->Stack + Task->StackPoint;

    return fsm_call_sub_ex(
            State, Arg, ptRoutine->State, ptRoutine->Arg);
}

/*! \brief get a new task control block from pool and initial it
 *  \param State task's first routine
 *  \param Arg a pointer of a argument
 *  \param Stack a return stack buffer
 *  \param StackSize return stack size
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
static fsm_tcb_t *fsm_tcb_new(
        state_func_t *      State,
        void *              Arg,
        task_stack_t *      Stack,
        uint_fast8_t        StackSize)
{
    fsm_tcb_t *pTCB;
    
    if (NULL != sptTCBFreeList) {       //!< Get a TCB.
        pTCB = sptTCBFreeList;
        sptTCBFreeList = sptTCBFreeList->Next;
    } else {
        return NULL;
    }

    pTCB->Next              = NULL;
    
    pTCB->Flag              = 0;
    pTCB->Status            = FSM_TASK_STATUS_INVALID;

    pTCB->Stack             = Stack;        //!< set stack buffer
    pTCB->StackSize         = StackSize;   //!< set stack size
    pTCB->StackPoint        = 0;             //!< set stack point
    pTCB->Stack[0].State    = State;       //!< set task routine
    pTCB->Stack[0].Arg      = Arg;          //!< set argument

#if SAFE_TASK_THREAD_SYNC == ENABLED
    pTCB->Object            = NULL;
#endif

    return pTCB;
}

/*! \brief  free a task control blcok to pool
 *  \param  pTCB a pointer of a task control block
 *  \return none
 */
static void fsm_tcb_free(fsm_tcb_t *pTCB)
{
    MEM_SET_ZERO((void *)pTCB, sizeof(fsm_tcb_t));

    pTCB->Next     = sptTCBFreeList;        //! add task item to freelist
    sptTCBFreeList  = pTCB;
}

/*! \brief  create a new task control block
 *  \param  State task routine
 *  \param  fnReturnTo a routine which will be called when task routine return
 *          false.
 *  \param  Arg a pointer of a argument control block
 *  \param  Stack a return stack buffer
 *  \param  StackSize return stack size
 *  \return task handle
 */
uint_fast8_t fsm_task_create(
        fsm_tcb_t **        pptTask,
        state_func_t *      State,
        void *              Arg,
        task_stack_t * Stack,
        uint_fast8_t        StackSize)
{
    fsm_tcb_t *ptTask;
    
    if ((NULL == pptTask)                       //! validate parameters.
    ||  (NULL == State)
    ||  (NULL == Stack) 
    ||  (0 == StackSize)) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    /*! try to get a TCB */
    ptTask = fsm_tcb_new(State, Arg, Stack, StackSize);
    if (NULL == ptTask) {
        return FSM_ERR_TASK_NO_MORE_TCB;
    }
    
    /*! Let this task to run. */
    fsm_set_task_ready(ptTask);

    *pptTask = ptTask;
    
    return FSM_ERR_NONE;
}

/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param Task a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
bool fsm_task_enqueue(task_queue_t *pTaskQueue, fsm_tcb_t *Task)
{
    SAFE_ATOM_CODE(
        Task->Next = NULL;
        if (pTaskQueue->Tail == NULL) {        //!< is this queue empty?
            pTaskQueue->Head = Task;          //!< add this new task to head
        } else {                                    //!< if queue is not empty
            pTaskQueue->Tail->Next = Task;   //!< add this new task to tail
        }
        pTaskQueue->Tail = Task;              //!< move tail to next.
    )

    return true;
}

/*! \brief  remove a task control block from a specified task control queue
 *  \param  pTaskQueue a pointer of a task queue control block
 *  \param  Task a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded to get a task from queue
 */
fsm_tcb_t *fsm_task_dequeue(task_queue_t *pTaskQueue)
{
    fsm_tcb_t *Task;

    SAFE_ATOM_CODE(
        Task = pTaskQueue->Head;
        if (Task != NULL) {                        //!< is this queue empty?
            pTaskQueue->Head = Task->Next;   //!< move head to next.
            if (pTaskQueue->Head == NULL) {    //!< is the queue empty?
                pTaskQueue->Tail = NULL;       //!< terminate tail.
            }
        }
    )

    return Task;
}

bool fsm_remove_task_from_queue(task_queue_t *pTaskQueue, fsm_tcb_t *Task)
{
    fsm_tcb_t **ppTCB;
    bool bRes = false;
    
    SAFE_ATOM_CODE(
        for (ppTCB = &pTaskQueue->Head; *ppTCB; ppTCB = &((*ppTCB)->Next)) {
            if (*ppTCB == Task) {
                *ppTCB = Task->Next;
                Task->Next = NULL;
                if (pTaskQueue->Head == NULL) {
                    pTaskQueue->Tail = NULL;
                } else {
                    for (Task = pTaskQueue->Head; Task->Next != NULL; Task = Task->Next);
                    pTaskQueue->Tail = Task;
                }
                bRes = true;
                break;
            }
        }
    )
    
    return bRes;
}

/*! \brief  add a task to ready table(list queue).
 *  \param  Task task control block
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
bool fsm_set_task_ready(fsm_tcb_t *Task)
{
    Task->Status = FSM_TASK_STATUS_READY;
    fsm_task_enqueue(&stScheduler.tReadyList, Task);  //!< just enqueue.

    return true;
}

/*! \brief  fsm_scheduler function
 *  \retval true system is busy
 *  \retval false system is idle, no task to run.
 */
bool fsm_scheduler(void)
{  
    fsm_tcb_t *         Task;
    task_stack_t * ptRoutine;

    /* get a task from queue */
    Task = fsm_task_dequeue(&stScheduler.tReadyList);
    if (NULL == Task) {
        return false;
    }
    stScheduler.ptCurrentTask = Task;

    ptRoutine = Task->Stack + Task->StackPoint;

    /* run task routine */
    ptRoutine->State(ptRoutine->Arg);

    do {
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        /* Is task waitting synchronization object to set? */
        if (NULL != Task->Object) {           //!< task is waitting synchronization object
            break;
        }
    #endif
        
        /* check whether this fsm is complete. */
        if (ptRoutine->State != NULL) {       
            fsm_set_task_ready(Task);          //!< re-add this task to queue
            break;
        } else {                                //!< it has returned from a sub-fsm.
            if (fsm_task_stack_pop(Task)) {    //!< has parent fsm?
                fsm_set_task_ready(Task);      //!< re-add this task to queue
                break;
            }
        }
        
        fsm_tcb_free(Task);            //!< this task finished, free task
    } while (0);

    return true;
}

bool fsm_register_object(void *Obj)
{
    fsm_basis_obj_t *Object = (fsm_basis_obj_t *)Obj;
    
    SAFE_ATOM_CODE(
        Object->ObjNext     = sObjRegistedList;
        sObjRegistedList    = Object;
    )

    return true;
}

bool fsm_deregister_object(void *Obj)
{
    fsm_basis_obj_t **ObjectRef;
    fsm_basis_obj_t *Object = (fsm_basis_obj_t *)Obj;
    bool bRes = false;

    SAFE_ATOM_CODE(
        for (ObjectRef = &sObjRegistedList; *ObjectRef != NULL; ObjectRef = &((*ObjectRef)->ObjNext)) {
            if (*ObjectRef == Object) {
                *ObjectRef = (*ObjectRef)->ObjNext;
                Object->ObjNext = NULL;
                bRes = true;
                break;
            }
        }
    )

    return bRes;
}

void fsm_time_tick(void)
{
    fsm_basis_obj_t *Obj;

    for (Obj = sObjRegistedList; Obj != NULL; Obj = Obj->ObjNext) {
        if (Obj->ObjType & FSM_OBJ_TYPE_WAITABLE) {   //!< only watiable object can be wait.
            fsm_waitable_obj_header_t *WatiableObj = (fsm_waitable_obj_header_t *)Obj;
            fsm_tcb_t *Task;
            fsm_tcb_t *NextTCB;
            for (Task = WatiableObj->Head; Task; Task = NextTCB) {
                NextTCB = Task->Next;
                if (Task->Delay != 0) {
                    Task->Delay--;
                    if (Task->Delay == 0) {
                        //! 1. Remove this task from this object's wait queue.
                        fsm_remove_task_from_queue(&(WatiableObj->TaskQueue), Task);
                        //! 2. set tcb.Object NULL
                        Task->Object = NULL;
                        //! 3. Let this task to ruan and chang it's status.
                        fsm_set_task_ready(Task);
                        Task->Status = FSM_TASK_STATUS_PEND_TIMEOUT;
                    }
                }
            }
        }
    }
}

#if SAFE_TASK_THREAD_SYNC == ENABLED
/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  Task parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
uint_fast8_t fsm_wait_for_single_object(void *Object, uint32_t wTimeout)
{
    uint8_t         chResult;
    uint8_t         ObjType;
    fsm_tcb_t *     Task = stScheduler.ptCurrentTask;

    if (FSM_IN_INT()) {
        return FSM_ERR_PEND_ISR;
    }
    
    if (NULL == Object) {          //!< fatal error!
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (Task->Status == FSM_TASK_STATUS_READY) {
        ObjType = ((fsm_basis_obj_t *)Object)->ObjType;
        if (!(ObjType & FSM_OBJ_TYPE_WAITABLE)) {
            return FSM_ERR_OBJ_NOT_WAITABLE;
        }
        
        chResult = FSM_ERR_OBJ_NOT_SINGLED;
        switch (ObjType) {
        case FSM_OBJ_TYPE_EVENT: {
            fsm_event_t *ptEvent = (fsm_event_t *)Object;
            SAFE_ATOM_CODE(
                if (ptEvent->EventFlag & FSM_EVENT_SINGNAL_BIT) {
                    if (!(ptEvent->EventFlag & FSM_EVENT_MANUAL_RESET_BIT)) {
                        ptEvent->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
                    }
                    chResult = FSM_ERR_NONE;
                } else {
                    //! add task to the object's wait queue.
                    Task->Object = (fsm_basis_obj_t *)ptEvent;
                    Task->Delay   = wTimeout;
                    Task->Status = FSM_TASK_STATUS_PEND;
                    fsm_task_enqueue(&(ptEvent->TaskQueue), Task);
                }
            )
            break;
        }
        case FSM_OBJ_TYPE_MUTEX: {
            fsm_mutex_t *ptMutex = (fsm_mutex_t *)Object;
            SAFE_ATOM_CODE(
                if (ptMutex->MutexFlag & FSM_MUTEX_OWNED_BIT) {
                    //! add task to the object's wait queue.
                    Task->Object = (fsm_basis_obj_t *)ptMutex;
                    Task->Delay   = wTimeout;
                    Task->Status = FSM_TASK_STATUS_PEND;
                    fsm_task_enqueue(&(ptMutex->TaskQueue), Task);
                } else {
                    ptMutex->MutexFlag |= FSM_MUTEX_OWNED_BIT;
                    chResult = FSM_ERR_NONE;
                }
            )
            break;
        }
        case FSM_OBJ_TYPE_SEM: {
            fsm_semaphore_t *ptSem = (fsm_semaphore_t *)Object;
            SAFE_ATOM_CODE(
                if (ptSem->SemCounter == 0) {
                    //! add task to the object's wait queue.
                    Task->Object = (fsm_basis_obj_t *)ptSem;
                    Task->Delay   = wTimeout;
                    Task->Status = FSM_TASK_STATUS_PEND;
                    fsm_task_enqueue(&(ptSem->TaskQueue), Task);
                } else {
                    ptSem->SemCounter--;
                    chResult = FSM_ERR_NONE;
                }
            )
            break;
        }
        case FSM_OBJ_TYPE_INVALID:
        default:
            break;
        }
    } else if (Task->Status == FSM_TASK_STATUS_PEND_OK) {
        Task->Status = FSM_TASK_STATUS_READY;
        Task->Delay   = 0;
        chResult = FSM_ERR_NONE;
    } else if (Task->Status == FSM_TASK_STATUS_PEND_TIMEOUT) {
        Task->Status = FSM_TASK_STATUS_READY;
        chResult = FSM_ERR_TASK_PEND_TIMEOUT;
    }

    return chResult;
}

#endif

/*! \brief  add task control blocks to the task pool
 *  \param  pHead task heap buffer
 *  \param  hwSize heap size
 *  \return access result
 */
static void fsm_tcb_pool_init(void)
{
    uint_fast8_t n;
    fsm_tcb_t **p;

    MEM_SET_ZERO((void *)stTCBs, sizeof(stTCBs));
    p = &sptTCBFreeList;
    
    //! add TCBs to the free list
    for (n = 0; n < UBOUND(stTCBs); n++) {
        *p = &stTCBs[n];
        p  = &((*p)->Next);
    }
}

/*! \brief fsm init.
 */
void fsm_init(void)
{
    gchFSMIntNesting = 0;
    fsm_tcb_pool_init();
    MEM_SET_ZERO((void *)&stScheduler, sizeof(stScheduler));
    fsm_event_init();
    fsm_mutex_init();
    fsm_semaphore_init();
}

/* EOF */
