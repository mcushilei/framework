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
static bool         fsm_task_stack_push        (fsm_tcb_t          *Task,
                                                fsm_state_t        *State,
                                                void               *Arg);
static bool         fsm_task_stack_pop         (fsm_tcb_t          *Task);
static void         fsm_tcb_pool_init          (void);
static fsm_tcb_t   *fsm_tcb_new                (void);
static void         fsm_tcb_free               (fsm_tcb_t          *Task);
static void         fsm_tcb_init               (fsm_tcb_t          *pTCB,
                                                fsm_state_t        *State,
                                                void               *Arg,
                                                task_stack_t       *Stack,
                                                uint8_t             StackSize);
static bool         fsm_task_enqueue           (fsm_task_list_t    *pTaskQueue,
                                                fsm_tcb_t          *pTask);
static fsm_tcb_t   *fsm_task_dequeue           (fsm_task_list_t    *pTaskQueue);
static bool         fsm_task_remove_from_queue (fsm_task_list_t    *pTaskQueue,
                                                fsm_tcb_t          *pTask);
static fsm_tcb_t   *fsm_get_next_ready_task    (void);

extern void         fsm_flag_init              (void);
extern void         fsm_mutex_init             (void);
extern void         fsm_semaphore_init         (void);

/*============================ LOCAL VARIABLES ===============================*/
static fsm_tcb_t       *fsmTCBFreeList;                 //! TCB
static fsm_tcb_t        fsmTCBTbl[FSM_MAX_TASKS];       //! TCB pool
uint8_t                 fsmIntNesting;
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
 *  \param State    target state. Transfering to a NULL state will terminal this FSM.
 *  \param Arg a    argument to the task.
 *  \retval false   invalid param
 *  \retval true    succeeded to transfer state
 */
bool fsm_state_transfer(fsm_state_t *State, void *Arg)
{
    fsm_tcb_t      *pTCB      = fsmScheduler.CurrentTask;
    task_stack_t   *ptRoutine = pTCB->Stack + pTCB->StackPoint;

    ptRoutine->State = State;
    ptRoutine->Arg   = Arg;

    return true;
}

/*! \brief  call a sub routine and return CURRENT state when sub fsm complete.
 *  \param  State target routine
 *  \param  Arg a pointer of argument control block
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
fsm_err_t fsm_call_sub(fsm_state_t *State, void *Arg)
{
    fsm_tcb_t      *Task = fsmScheduler.CurrentTask;

    if (!fsm_task_stack_push(Task, State, Arg)) {
        return FSM_ERR_TASK_STACK_FULL;
    }

    return FSM_ERR_NONE;
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
    pTCB->Status            = FSM_TASK_STATUS_READY;

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
    
    ptTask = fsm_tcb_new();         /*! try to get a TCB */
    if (NULL == ptTask) {
        return FSM_ERR_TASK_NO_MORE_TCB;
    }
    
    fsm_tcb_init(ptTask, State, Arg, Stack, StackSize);
    
    fsm_set_task_ready(ptTask, FSM_TASK_STATUS_READY);     /*! Let this task to run. */

    if (NULL != pptTask) {
        *pptTask = ptTask;
    }
    
    return FSM_ERR_NONE;
}

fsm_err_t fsm_time_delay(uint32_t wTimeout)
{
    uint8_t         chResult;
    fsm_tcb_t      *pTask = fsmScheduler.CurrentTask;

    if (fsmIntNesting != 0) {
        return FSM_ERR_PEND_ISR;
    }
    
    switch (pTask->Status) {
        case FSM_TASK_STATUS_READY:
            if (wTimeout == 0u) {
                chResult = FSM_ERR_TASK_PEND_TIMEOUT;
            } else {
                //! add task to the object's wait queue.
                pTask->Object = NULL;
                fsm_set_task_pend(wTimeout);
                chResult = FSM_ERR_OBJ_NOT_SINGLED;
            }
            break;
            
        case FSM_TASK_STATUS_PEND_OK:
        case FSM_TASK_STATUS_PEND_TIMEOUT:
            pTask->Status    = FSM_TASK_STATUS_READY;
            chResult         = FSM_ERR_TASK_PEND_TIMEOUT;
            break;
        
        case FSM_TASK_STATUS_PEND:
        default:
            chResult = FSM_ERR_OBJ_NOT_SINGLED;
            break;
    }

    return chResult;
}

void fsm_time_tick(void)
{
    fsm_tcb_t      *pTask, *pNextTask;
        
    FSM_SAFE_ATOM_CODE(
        for (pTask = fsmScheduler.PendList.Head; pTask != NULL; pTask = pNextTask) {
            pNextTask = pTask->Next;
            if ((pTask->Delay != 0) && (pTask->Delay != FSM_INFINITE)) {
               pTask->Delay--;
               if (pTask->Delay == 0) {
                   if (pTask->Object != NULL) {
                       fsm_waitable_obj_remove_task(pTask->Object, pTask);
                   }
                   fsm_set_task_ready(pTask, FSM_TASK_STATUS_PEND_TIMEOUT);
               }
            }
        }
    )
}


/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param Task a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
static bool fsm_task_enqueue(fsm_task_list_t *pTaskQueue, fsm_tcb_t *pTask)
{
    pTask->Next = NULL;
    pTask->Prev = NULL;
    if (pTaskQueue->Tail == NULL) {         //!< is this queue empty?
        pTaskQueue->Head = pTask;            //!< add this new task to head
    } else {                                //!< if queue is not empty
        pTaskQueue->Tail->Next = pTask;      //!< add this new task to tail
        pTask->Prev             = pTaskQueue->Tail;
    }
    pTaskQueue->Tail = pTask;                //!< move tail to next.

    return true;
}

/*! \brief  remove a task control block from a specified task control queue
 *  \param  pTaskQueue a pointer of a task queue control block
 *  \param  Task a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded to get a task from queue
 */
static fsm_tcb_t *fsm_task_dequeue(fsm_task_list_t *pTaskQueue)
{
    fsm_tcb_t *pTask;

    pTask = pTaskQueue->Head;
    if (pTask != NULL) {                     //!< is this queue empty?
        pTaskQueue->Head = pTask->Next;      //!< move head to next.
        if (pTaskQueue->Head == NULL) {     //!< is the queue empty?
            pTaskQueue->Tail = NULL;        //!< terminate tail.
        } else {
            pTaskQueue->Head->Prev = NULL;
        }
        
        pTask->Next = NULL;
        pTask->Prev = NULL;
    }

    return pTask;
}

static bool fsm_task_remove_from_queue(fsm_task_list_t *pTaskQueue, fsm_tcb_t *pTask)
{
    fsm_tcb_t *pPrev, *pNext;
    
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
    
    return true;
}

/*! \brief  add a task to ready list queue.
 *  \param  Task task control block
 */
void fsm_set_task_ready(fsm_tcb_t *pTask, uint8_t pendStat)
{
    FSM_SAFE_ATOM_CODE(
        pTask->Status = pendStat;
        pTask->Object = NULL;
        fsm_task_remove_from_queue(&fsmScheduler.PendList, pTask);
        fsm_task_enqueue(&fsmScheduler.ReadyList, pTask);
    )
}

void fsm_set_task_pend (uint32_t timeDelay)
{
    FSM_SAFE_ATOM_CODE(
        fsmScheduler.CurrentTask->Delay    = timeDelay;
        fsmScheduler.CurrentTask->Status   = FSM_TASK_STATUS_PEND;
        fsm_task_enqueue(&fsmScheduler.PendList, fsmScheduler.CurrentTask);
    )
}

static fsm_tcb_t *fsm_get_next_ready_task(void)
{
    fsm_tcb_t *pTask;
    
    FSM_SAFE_ATOM_CODE(
        pTask = fsm_task_dequeue(&fsmScheduler.ReadyList);
    )
        
    return pTask;
}


void fsm_waitable_obj_add_task(fsm_waitable_obj_t *pObj, fsm_tcb_t *pTask)
{
    pTask->WaitNodeNext = NULL;
    pTask->WaitNodePrev = NULL;
    if (pObj->TaskQueue.Tail == NULL) {         //!< is this queue empty?
        pObj->TaskQueue.Head = pTask;            //!< add this new task to head
    } else {                                //!< if queue is not empty
        pObj->TaskQueue.Tail->Next  = pTask;      //!< add this new task to tail
        pTask->WaitNodePrev         = pObj->TaskQueue.Tail;
    }
    pObj->TaskQueue.Tail = pTask;                //!< move tail to next.
}

fsm_tcb_t *fsm_waitable_obj_get_task(fsm_waitable_obj_t *pObj)
{
    fsm_tcb_t *pTask;

    pTask = pObj->TaskQueue.Head;
    if (pTask != NULL) {                     //!< is this queue empty?
        pObj->TaskQueue.Head = pTask->WaitNodeNext;      //!< move head to next.
        if (pObj->TaskQueue.Head == NULL) {     //!< is the queue empty?
            pObj->TaskQueue.Tail = NULL;        //!< terminate tail.
        } else {
            pObj->TaskQueue.Head->WaitNodePrev = NULL;
        }
        
        pTask->WaitNodeNext = NULL;
        pTask->WaitNodePrev = NULL;
    }

    return pTask;
}

void fsm_waitable_obj_remove_task(fsm_waitable_obj_t *pObj, fsm_tcb_t *pTask)
{
    fsm_tcb_t *pPrev, *pNext;
    
    pPrev = pTask->WaitNodePrev;
    pNext = pTask->WaitNodeNext;

    if (pPrev != NULL) {
        pPrev->WaitNodeNext = pNext;
    }
    if (pNext != NULL) {
        pNext->WaitNodePrev = pPrev;
    }
               
    if (pTask == pObj->TaskQueue.Head) {
        pObj->TaskQueue.Head = pNext;
    }
    if (pTask == pObj->TaskQueue.Tail) {
        pObj->TaskQueue.Tail = pPrev;
    }
               
    pTask->WaitNodeNext = NULL;
    pTask->WaitNodePrev = NULL;
    pTask->Object       = NULL;
}

/*! \brief  fsm_scheduler function
 *  \retval true system is busy
 *  \retval false system is idle, no task to run.
 */
bool fsm_scheduler(void)
{  
    fsm_tcb_t      *pTask;
    task_stack_t   *ptRoutine;

    pTask = fsm_get_next_ready_task();
    if (NULL == pTask) {
        return false;
    }
    fsmScheduler.CurrentTask = pTask;

    ptRoutine = pTask->Stack + pTask->StackPoint;
    ptRoutine->State(ptRoutine->Arg);                   //!< run state routine.

    do {
        if (pTask->Status == FSM_TASK_STATUS_PEND) {    //!< Task has been pended?
            break;                                      //!< Yes, this task has been moved to pend list.
        }
        
                                                        //!< No.
        if (ptRoutine->State != NULL) {                         //!< Whether this fsm is complete.
            fsm_set_task_ready(pTask, FSM_TASK_STATUS_READY);   //!< No.
        } else if (fsm_task_stack_pop(pTask)) {                 //!< Yes, Has parent fsm?
            fsm_set_task_ready(pTask, FSM_TASK_STATUS_READY);   //!< Yes, re-add this task to queue
        } else {
            fsm_tcb_free(pTask);                                //!< No, this task finished, free task
        }
    } while (0);

    return true;
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
    fsmScheduler.CurrentTask    = NULL;
    fsmScheduler.PendList.Head  = NULL;
    fsmScheduler.PendList.Tail  = NULL;
    fsmScheduler.ReadyList.Head = NULL;
    fsmScheduler.ReadyList.Tail = NULL;
    fsm_tcb_pool_init();
    fsm_flag_init();
    fsm_mutex_init();
    fsm_semaphore_init();
}

/* EOF */
