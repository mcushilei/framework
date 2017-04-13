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
/*============================ PROTOTYPES ====================================*/
static bool         fsm_task_stack_push         (
                                                fsm_tcb_t          *Task,
                                                fsm_state_t       *State,
                                                void               *Arg);
static bool         fsm_task_stack_pop          (fsm_tcb_t         *Task);
static void         fsm_tcb_pool_init           (void);
static fsm_tcb_t   *fsm_tcb_new                 (void);
static void         fsm_tcb_free                (fsm_tcb_t         *Task);
static void         fsm_tcb_init                (
                                                fsm_tcb_t          *pTCB,
                                                fsm_state_t        *State,
                                                void               *Arg,
                                                task_stack_t       *Stack,
                                                uint8_t             StackSize);

extern void         fsm_flag_init               (void);
extern void         fsm_mutex_init              (void);
extern void         fsm_semaphore_init          (void);

/*============================ LOCAL VARIABLES ===============================*/
static fsm_tcb_t       *fsmTCBFreeList;                 //! TCB
static fsm_tcb_t        fsmTCBTbl[FSM_MAX_TASKS];       //! TCB pool
volatile uint8_t        fsmIntNesting;
scheduler_t             fsmScheduler;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_int_enter(void)
{
    if (fsmIntNesting < 255u) {
        fsmIntNesting++;
    }
}
                
void fsm_int_exit(void)
{
    if (fsmIntNesting != 0) {
        fsmIntNesting--;
    }
}
                
/*! \brief try to push a routine into stack
 *  \param Task
 *  \param State routine
 *  \param Arg argument for routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
static bool fsm_task_stack_push(
        fsm_tcb_t      *Task,
        fsm_state_t    *State,
        void           *Arg)
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
bool fsm_state_transfer(fsm_state_t *State, void *Arg)
{
    fsm_tcb_t      *Task     = fsmScheduler.CurrentTask;
    task_stack_t   *ptRoutine = Task->Stack + Task->StackPoint;

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
fsm_err_t fsm_call_sub_ex(
        fsm_state_t    *State,
        void           *Arg,
        fsm_state_t    *ReturnState,
        void           *ReturnArg)
{
    fsm_tcb_t *Task = fsmScheduler.CurrentTask;

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
fsm_err_t fsm_call_sub(fsm_state_t *State, void *Arg)
{
    fsm_tcb_t      *Task = fsmScheduler.CurrentTask;
    task_stack_t   *ptRoutine = Task->Stack + Task->StackPoint;

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
static fsm_tcb_t *fsm_tcb_new(void)
{
    fsm_tcb_t *pTCB;
    
    if (NULL != fsmTCBFreeList) {       //!< Get a TCB.
        pTCB = fsmTCBFreeList;
        fsmTCBFreeList = fsmTCBFreeList->Next;
    } else {
        return NULL;
    }

    return pTCB;
}

/*! \brief  free a task control blcok to pool
 *  \param  pTCB a pointer of a task control block
 *  \return none
 */
static void fsm_tcb_free(fsm_tcb_t *pTCB)
{
    MEM_SET_ZERO((void *)pTCB, sizeof(fsm_tcb_t));

    pTCB->Next      = fsmTCBFreeList;        //! add tcb to freelist
    fsmTCBFreeList  = pTCB;
}

static void fsm_tcb_init(
        fsm_tcb_t          *pTCB,
        fsm_state_t        *State,
        void               *Arg,
        task_stack_t       *Stack,
        uint8_t             StackSize)
{
    pTCB->Next              = NULL;
    pTCB->Prev              = NULL;
    
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
fsm_err_t fsm_task_create(
        fsm_tcb_t         **pptTask,
        fsm_state_t        *State,
        void               *Arg,
        task_stack_t       *Stack,
        uint8_t             StackSize)
{
    fsm_tcb_t *ptTask;
    
    //! validate parameters.
    if ((NULL == State)
    ||  (NULL == Stack) 
    ||  (0 == StackSize)) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    /*! try to get a TCB */
    ptTask = fsm_tcb_new();
    if (NULL == ptTask) {
        return FSM_ERR_TASK_NO_MORE_TCB;
    }
    
    fsm_tcb_init(ptTask, State, Arg, Stack, StackSize);
    
    /*! Let this task to run. */
    fsm_set_task_ready(ptTask);

    if (NULL != pptTask) {
        *pptTask = ptTask;
    }
    
    return FSM_ERR_NONE;
}

/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param Task a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
bool fsm_task_enqueue(fsm_task_list_t *pTaskQueue, fsm_tcb_t *Task)
{
    SAFE_ATOM_CODE(
        Task->Next = NULL;
        Task->Prev = NULL;
        if (pTaskQueue->Tail == NULL) {         //!< is this queue empty?
            pTaskQueue->Head = Task;            //!< add this new task to head
        } else {                                //!< if queue is not empty
            pTaskQueue->Tail->Next = Task;      //!< add this new task to tail
            Task->Prev             = pTaskQueue->Tail;
        }
        pTaskQueue->Tail = Task;                //!< move tail to next.
    )

    return true;
}

/*! \brief  remove a task control block from a specified task control queue
 *  \param  pTaskQueue a pointer of a task queue control block
 *  \param  Task a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded to get a task from queue
 */
fsm_tcb_t *fsm_task_dequeue(fsm_task_list_t *pTaskQueue)
{
    fsm_tcb_t *Task;

    SAFE_ATOM_CODE(
        Task = pTaskQueue->Head;
        if (Task != NULL) {                     //!< is this queue empty?
            pTaskQueue->Head = Task->Next;      //!< move head to next.
            if (pTaskQueue->Head == NULL) {     //!< is the queue empty?
                pTaskQueue->Tail = NULL;        //!< terminate tail.
            } else {
                pTaskQueue->Head->Prev = NULL;
            }
            
            Task->Next = NULL;
            Task->Prev = NULL;
        }
    )

    return Task;
}

bool fsm_remove_task_from_queue(fsm_task_list_t *pTaskQueue, fsm_tcb_t *pTask)
{
    fsm_tcb_t *pPrev, *pNext;
    
    SAFE_ATOM_CODE(
        pPrev = pTask->Prev;
        pNext = pTask->Next;

        if (pPrev != NULL) {
            pPrev->Next = pNext;
        }
        if (pNext != NULL) {
            pNext->Prev = pPrev;
        }
                   
        if (pTask == pTaskQueue->Head) {
            pTaskQueue->Head = pNext;
        }
        if (pTask == pTaskQueue->Tail) {
            pTaskQueue->Tail = pPrev;
        }
                   
        pTask->Next = NULL;
        pTask->Prev = NULL;
    )
    
    return true;
}

void fsm_waitable_obj_add_task(fsm_waitable_obj_t *pObj, fsm_tcb_t *pTask)
{
}

void fsm_waitable_obj_remove_task(fsm_waitable_obj_t *pObj, fsm_tcb_t *pTask)
{
}

/*! \brief  add a task to ready table(list queue).
 *  \param  Task task control block
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
void fsm_set_task_ready(fsm_tcb_t *pTask)
{
    pTask->Status = FSM_TASK_STATUS_READY;
    fsm_task_enqueue(&fsmScheduler.ReadyList, pTask);  //!< just enqueue.
}

void fsm_set_task_pend (fsm_tcb_t          *pTask,
                        fsm_handle_t        hObj)
{
    pTask->Object = hObj;
    fsm_task_enqueue(&fsmScheduler.PendList, pTask);
}

/*! \brief  fsm_scheduler function
 *  \retval true system is busy
 *  \retval false system is idle, no task to run.
 */
bool fsm_scheduler(void)
{  
    fsm_tcb_t      *Task;
    task_stack_t   *ptRoutine;

    /* get a task from queue */
    Task = fsm_task_dequeue(&fsmScheduler.ReadyList);
    if (NULL == Task) {
        return false;
    }
    fsmScheduler.CurrentTask = Task;

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

void fsm_time_tick(void)
{
}

/*! \brief  add task control blocks to the task pool
 *  \param  pHead task heap buffer
 *  \param  hwSize heap size
 *  \return access result
 */
static void fsm_tcb_pool_init(void)
{
    uint_fast16_t n;
    fsm_tcb_t **p;

    MEM_SET_ZERO((void *)fsmTCBTbl, sizeof(fsmTCBTbl));
    p = &fsmTCBFreeList;
    
    //! add TCBs to the free list
    for (n = 0; n < ARRAY_LENGTH(fsmTCBTbl); n++) {
        *p = &fsmTCBTbl[n];
        p  = &((*p)->Next);
    }
}

/*! \brief fsm init.
 */
void fsm_init(void)
{
    fsmIntNesting = 0;
    fsm_tcb_pool_init();
    MEM_SET_ZERO((void *)&fsmScheduler, sizeof(fsmScheduler));
    fsm_flag_init();
    fsm_mutex_init();
    fsm_semaphore_init();
}

/* EOF */
