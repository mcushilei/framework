
#include ".\os.h"

/*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************/

static  void    OS_TaskDel     (void);

static  void    OS_TaskReturn   (void);

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
static  void    OS_TaskStkClr  (OS_STK         *pbos,
                                INT32U          size,
                                INT8U           opt);
#endif


/*********************************************************************************************************
*                                      CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : oldp     is the old priority
*
*              newp     is the new priority
*
* Returns    : OS_ERR_NONE            is the call was successful
*              OS_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*              OS_ERR_PRIO_EXIST      if the new priority already exist.
*              OS_ERR_TASK_NOT_EXIST  there is no task with the specified OLD priority (i.e. the OLD task does
*                                     not exist.
*              OS_ERR_PRIO            if the task is assigned to a Mutex PIP.
*********************************************************************************************************/

#if OS_TASK_CHANGE_PRIO_EN > 0u
INT8U  osTaskChangePrio (INT8U  oldprio,
                         INT8U  newprio)
{
#if (OS_EVENT_EN)
    OS_EVENT  *pevent;
#endif
    OS_TCB    *ptcb;
    INT8U      y_new;
    INT8U      x_new;
    INT8U      y_old;
    OS_PRIO    bity_new;
    OS_PRIO    bitx_new;
    OS_PRIO    bity_old;
    OS_PRIO    bitx_old;
#if OS_CRITICAL_METHOD == 3u
    OS_CPU_SR  cpu_sr = 0u;                                 //!< Storage for CPU status register
#endif


#if OS_ARG_CHK_EN > 0u
    if (oldprio >= OS_MAX_PRIO_LEVELS) {
        if (oldprio != OS_PRIO_SELF) {
            return OS_ERR_PRIO_INVALID;
        }
    }
    if (newprio >= OS_MAX_PRIO_LEVELS) {
        return OS_ERR_PRIO_INVALID;
    }
#endif

    OS_ENTER_CRITICAL();
    if (OSTaskPrioTCBTbl[newprio] != OS_NULL) {             //!< New priority must not already exist
        OS_EXIT_CRITICAL();
        return OS_ERR_PRIO_EXIST;
    }
    if (oldprio == OS_PRIO_SELF) {                          //!< See if changing self
        oldprio = OSTCBCur->OSTCBPrio;                      //!< Yes, get priority
    }
    ptcb = OSTaskPrioTCBTbl[oldprio];
    if (ptcb == OS_NULL) {                                  //!< Does task to change exist?
        OS_EXIT_CRITICAL();                                 //!< No, can't change its priority!
        return OS_ERR_TASK_NOT_EXIST;
    }
    if (ptcb == OS_TCB_RESERVED) {                          //!< Is task assigned to Mutex
        OS_EXIT_CRITICAL();                                 //!< No, can't change its priority!
        return OS_ERR_PRIO;
    }
#if OS_MAX_PRIO_LEVELS <= 64u
    y_new                 = (INT8U)(newprio >> 3u);         //!< Yes, compute new TCB fields
    x_new                 = (INT8U)(newprio & 0x07u);
#else
    y_new                 = (INT8U)((INT8U)(newprio >> 4u) & 0x0Fu);
    x_new                 = (INT8U)(newprio & 0x0Fu);
#endif
    bity_new              = (OS_PRIO)(1u << y_new);
    bitx_new              = (OS_PRIO)(1u << x_new);

    OSTaskPrioTCBTbl[oldprio] = OS_NULL;                        //!< Remove TCB from old priority
    OSTaskPrioTCBTbl[newprio] =  ptcb;                          //!< Place pointer to TCB @ new priority
    y_old                 =  ptcb->OSTCBY;
    bity_old              =  ptcb->OSTCBBitY;
    bitx_old              =  ptcb->OSTCBBitX;
    if ((OSRdyTbl[y_old] &   bitx_old) != 0u) {             //!< If task is ready make it not
         OSRdyTbl[y_old] &= (OS_PRIO)~bitx_old;
         if (OSRdyTbl[y_old] == 0u) {
             OSRdyGrp &= (OS_PRIO)~bity_old;
         }
         OSRdyGrp        |= bity_new;                       //!< Make new priority ready to run
         OSRdyTbl[y_new] |= bitx_new;
    }

#if (OS_EVENT_EN)
    pevent = ptcb->OSTCBEventPtr;
    if (pevent != OS_NULL) {
        pevent->OSEventTbl[y_old] &= (OS_PRIO)~bitx_old;    //!< Remove old task prio from wait list
        if (pevent->OSEventTbl[y_old] == 0u) {
            pevent->OSEventGrp    &= (OS_PRIO)~bity_old;
        }
        pevent->OSEventGrp        |= bity_new;              //!< Add    new task prio to   wait list
        pevent->OSEventTbl[y_new] |= bitx_new;
    }
#endif

    ptcb->OSTCBPrio = newprio;                              //!< Set new task priority
    ptcb->OSTCBY    = y_new;
    ptcb->OSTCBX    = x_new;
    ptcb->OSTCBBitY = bity_new;
    ptcb->OSTCBBitX = bitx_new;
    OS_EXIT_CRITICAL();
    if (OSRunning == OS_TRUE) {
        OS_Sched();                                         //!< Find new highest priority task
    }
    return OS_ERR_NONE;
}
#endif

/*********************************************************************************************************
*                                  CREATE A TASK (Extended Version)
*
* Description: This function is used to have OS manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.  This function is similar to osTaskCreate() except that it allows
*              additional information about a task to be specified.
*
* Arguments  : task      is a pointer to the task's code
*
*              parg     is a pointer to an optional data area which can be used to pass parameters to
*                        the task when the task first executes.  Where the task is concerned it thinks
*                        it was invoked and passed the argument 'parg' as follows:
*
*                            void task (void *parg)
*                            {
*                                for (;;) {
*                                    Task code;
*                                }
*                            }
*
*              prio      is the task's priority.  A unique priority MUST be assigned to each task and the
*                        lower the number, the higher the priority.
*
*              pstk      point to the LOWEST (valid) memory location of the stack.
*
*              stk_size  is the size of the stack in number of elements.  If OS_STK is set to INT8U,
*                        'stk_size' corresponds to the number of bytes available.  If OS_STK is set to
*                        INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                        OS_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                        available on the stack.
*
*              opt       contains additional information (or options) about the behavior of the task.  The
*                        LOWER 8-bits are reserved by OS while the upper 8 bits can be application
*                        specific.  See OS_TASK_OPT_??? in OS.H.  Current choices are:
*
*                        OS_TASK_OPT_STK_CHK      Stack checking to be allowed for the task
*                        OS_TASK_OPT_STK_CLR      Clear the stack when the task is created
*                        OS_TASK_OPT_SAVE_FP      If the CPU has floating-point registers, save them
*                                                 during a context switch.
*
* Returns    : OS_ERR_NONE                      if the function was successful.
*              OS_ERR_PRIO_EXIST                if the task priority already exist
*                                               (each task MUST have a unique priority).
*              OS_ERR_PRIO_INVALID              if the priority you specify is higher that the maximum
*              OS_ERR_TASK_CREATE_ISR           if you tried to create a task from an ISR.
*              OS_ERR_ILLEGAL_CREATE_RUN_TIME   if you tried to create a task after safety critical
*                                               operation started.
*********************************************************************************************************/

INT8U  osTaskCreate(void   (*task)(void *parg),
                    void    *parg,
                    INT8U    prio,
                    OS_STK  *pstk,
                    INT32U   stk_size,
                    INT8U    opt)
{
    OS_STK     *psp;
    INT8U       err;
#if OS_CRITICAL_METHOD == 3u                 //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    if (OSIntNesting > 0u) {                 //!< Make sure we don't create the task from within an ISR
        return OS_ERR_TASK_CREATE_ISR;
    }
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_MAX_PRIO_LEVELS) {             //!< Make sure priority is within allowable range
        return OS_ERR_PRIO_INVALID;
    }
#endif

    OS_ENTER_CRITICAL();
    if (OSTaskPrioTCBTbl[prio] == OS_NULL) {      //!< Make sure task doesn't already exist at this priority
        OSTaskPrioTCBTbl[prio] = OS_TCB_RESERVED; //!< Reserve the priority to prevent others from doing ...
                                                    //!< ... the same thing until task is created.
        OS_EXIT_CRITICAL();

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
        OS_TaskStkClr(pstk, stk_size, opt);                     //!< Clear the task stack (if needed)
#endif

#if OS_STK_GROWTH == 1u                                         //!< Initialize the task's stack
        psp = OSTaskStkInit((void *)task, parg, pstk + stk_size - 1u, (void *)&OS_TaskReturn);
#else
        psp = OSTaskStkInit((void *)task, parg, pstk, (void *)&OS_TaskReturn);
#endif
        err = OS_TCBInit(prio, psp, pstk, stk_size, opt);
        if (err == OS_ERR_NONE) {
            if (OSRunning == OS_TRUE) {                        //!< Find HPT if multitasking has started
                OS_Sched();
            }
        } else {
            OS_ENTER_CRITICAL();
            OSTaskPrioTCBTbl[prio] = OS_NULL;                  //!< Make this priority avail. to others
            OS_EXIT_CRITICAL();
        }
        return err;
    } else {
        OS_EXIT_CRITICAL();
        return OS_ERR_PRIO_EXIST;
    }
}

/*********************************************************************************************************
*                                            QUERY A TASK
*
* Description: This function is called to obtain a copy of the desired task's TCB.
*
* Arguments  : prio         is the priority of the task to obtain information from.
*
*              p_task_data  is a pointer to where the desired task's OS_TCB will be stored.
*
* Returns    : OS_ERR_NONE            if the requested task is suspended
*              OS_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*              OS_ERR_PRIO            if the desired task has not been created
*              OS_ERR_TASK_NOT_EXIST  if the task is assigned to a Mutex PIP
*              OS_ERR_PDATA_NULL      if 'p_task_data' is a NULL pointer
*********************************************************************************************************/

#if OS_TASK_QUERY_EN > 0u
INT8U  osTaskQuery (INT8U    prio,
                    OS_TCB  *p_task_data)
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_MAX_PRIO_LEVELS) {           //!< Task priority valid ?
        if (prio != OS_PRIO_SELF) {
            return OS_ERR_PRIO_INVALID;
        }
    }
    if (p_task_data == OS_NULL) {               //!< Validate 'p_task_data'
        return OS_ERR_PDATA_NULL;
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                 //!< See if query SELF
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTaskPrioTCBTbl[prio];
    if (ptcb == OS_NULL) {                      //!< Task to query must exist
        OS_EXIT_CRITICAL();
        return OS_ERR_PRIO;
    }
    if (ptcb == OS_TCB_RESERVED) {              //!< Task to query must has been create complete
        OS_EXIT_CRITICAL();
        return OS_ERR_TASK_NOT_EXIST;
    }
                                                //!< Copy TCB into user storage area
    OS_MemCopy((INT8U *)p_task_data, (INT8U *)ptcb, sizeof(OS_TCB));
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
#endif

/*********************************************************************************************************
*                                          DELETE CURRENT TASK
*
* Description: This function delete current running task. The deleted task is returned to the dormant
*              state and can be re-activated by creating the deleted task again. This function is
*              internal to OS and called by OS_TaskReturn. Your application should delete a task by return.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) To reduce interrupt latency, OS_TaskDel() 'disables' the task:
*                    a) by making it not ready
*                    b) by removing it from any wait lists
*                    c) by preventing osTimeTick() from making the task ready to run.
*                 The task can then be 'unlinked' from the miscellaneous structures in OS.
*              2) The function OS_Dummy() is called after OS_EXIT_CRITICAL() because, on most processors,
*                 the next instruction following the enable interrupt instruction is ignored.
*              3) An ISR cannot delete a task.
*              4) The lock nesting counter is incremented because, for a brief instant, if the current
*                 task is being deleted, the current task would not be able to be rescheduled because it
*                 is removed from the ready list.  Incrementing the nesting counter prevents another task
*                 from being schedule.  This means that an ISR would return to the current task which is
*                 being deleted.  The rest of the deletion would thus be able to be completed.
*********************************************************************************************************/

static  void    OS_TaskDel  (void)
{
    INT8U       prio;
    OS_TCB     *ptcb;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    OS_ENTER_CRITICAL();
    prio = OSTCBCur->OSTCBPrio;                         //!< Set priority to delete to current
    ptcb = OSTaskPrioTCBTbl[prio];

    OSRdyTbl[ptcb->OSTCBY] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[ptcb->OSTCBY] == 0u) {                 //!< Make task not ready
        OSRdyGrp           &= (OS_PRIO)~ptcb->OSTCBBitY;
    }

#if (OS_EVENT_EN)
    if (ptcb->OSTCBEventPtr != OS_NULL) {
        OS_EventTaskRemove(ptcb, ptcb->OSTCBEventPtr);  //!< Remove this task from any event   wait list
    }
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
    if (ptcb->OSTCBFlagNode != OS_NULL) {
        OS_FlagTaskRemove(ptcb->OSTCBFlagNode);         //!< Remove this task from any flag    wait list
    }
#endif

    ptcb->OSTCBDly      = 0u;                           //!< Prevent osTimeTick() from updating
    ptcb->OSTCBStat     = OS_STAT_RDY;                  //!< Prevent task from being resumed
    ptcb->OSTCBStatPend = OS_STAT_PEND_OK;
    if (OSLockNesting < 255u) {                         //!< Make sure we don't context switch
        OSLockNesting++;
    }
    OS_EXIT_CRITICAL();                                 //!< Enabling INT. ignores next instruc.
    OS_Dummy();                                         //!< ... Dummy ensures that INTs will be
    OS_ENTER_CRITICAL();                                //!< ... disabled HERE!
    if (OSLockNesting > 0u) {                           //!< Remove context switch lock
        OSLockNesting--;
    }

    OSTaskCtr--;                                        //!< One less task being managed
    OSTaskPrioTCBTbl[prio] = OS_NULL;                       //!< Clear old priority entry
    
                                                        //!< Remove from task list
    if (ptcb->OSTCBPrev == OS_NULL) {                   //!< Is this the first TCB of task list?
        ptcb->OSTCBNext->OSTCBPrev = OS_NULL;           //!< Yes.
        OSTaskList                 = ptcb->OSTCBNext;
    } else {
        ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
        ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
    }

    ptcb->OSTCBNext     = OSTCBFreeList;                //!< Return TCB to free TCB list
    OSTCBFreeList       = ptcb;
    OS_EXIT_CRITICAL();
    OS_Sched();                                     //!< Find new highest priority task
}

/*********************************************************************************************************
*                                    CATCH ACCIDENTAL TASK RETURN
*
* Description: This function is called if a task returns.  In other words, a task should either be an
*              infinite loop or delete itself if it's return.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

static  void    OS_TaskReturn   (void)
{
#if OS_CPU_HOOKS_EN > 0
    OSTaskReturnHook(OSTCBCur);     //!< Call hook to let user decide on what to do
#endif

    OS_TaskDel();                   //!< Delete task if it returns!
}

/*********************************************************************************************************
*                                          CLEAR TASK STACK
*
* Description: This function is used to clear the stack of a task (i.e. write all zeros)
*
* Arguments  : pbos     is a pointer to the task's bottom of stack.  If the configuration constant
*                       OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                       memory to low memory).  'pbos' will thus point to the lowest (valid) memory
*                       location of the stack.  If OS_STK_GROWTH is set to 0, 'pbos' will point to the
*                       highest memory location of the stack and the stack will grow with increasing
*                       memory locations.  'pbos' MUST point to a valid 'free' data item.
*
*              size     is the number of 'stack elements' to clear.
*
*              opt      contains additional information (or options) about the behavior of the task.  The
*                       LOWER 8-bits are reserved by OS while the upper 8 bits can be application
*                       specific.  See OS_TASK_OPT_??? in OS.H.
*
* Returns    : none
*********************************************************************************************************/

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
static  void    OS_TaskStkClr  (OS_STK  *pstk,
                                INT32U   size,
                                INT8U    opt)
{
    if ((opt & OS_TASK_OPT_STK_CHK) != 0x00u) {      //!< See if stack checking has been enabled
        if ((opt & OS_TASK_OPT_STK_CLR) != 0x00u) {  //!< See if stack needs to be cleared
            while (size > 0u) {
                size--;
                *pstk++ = (OS_STK)0;
            }
        }
    }
}
#endif

/*********************************************************************************************************
*                                           STACK CHECKING
*
* Description: This function is called to check the amount of free memory left on the specified task's
*              stack.
*
* Arguments  : prio          is the task priority
*
* Returns    : OS_ERR_NONE            upon success
*              OS_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*              OS_ERR_TASK_NOT_EXIST  if the desired task has not been created or is assigned to a Mutex PIP
*              OS_ERR_TASK_OPT        if you did NOT specified OS_TASK_OPT_STK_CHK when the task was created
*              OS_ERR_PDATA_NULL      if 'p_stk_data' is a NULL pointer
*********************************************************************************************************/

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
INT8U   OS_TaskStkChk   (INT8U  prio)
{
    OS_TCB    *ptcb;
    OS_STK    *pstk;
    INT32U     nfree;
    INT32U     size;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_MAX_PRIO_LEVELS) {                   //!< Make sure task priority is valid
        if (prio != OS_PRIO_SELF) {
            return OS_ERR_PRIO_INVALID;
        }
    }
#endif

    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) {                         //!< See if check for SELF
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTaskPrioTCBTbl[prio];
    if (ptcb == OS_NULL) {                              //!< Make sure task exist
        OS_EXIT_CRITICAL();
        return OS_ERR_TASK_NOT_EXIST;
    }
    if (ptcb == OS_TCB_RESERVED) {
        OS_EXIT_CRITICAL();
        return OS_ERR_TASK_NOT_EXIST;
    }
    if ((ptcb->OSTCBOpt & OS_TASK_OPT_STK_CHK) == 0u) { //!< Make sure stack checking option is set
        OS_EXIT_CRITICAL();
        return OS_ERR_TASK_OPT;
    }
    size  = ptcb->OSTCBStkSize;
    pstk  = ptcb->OSTCBStkBase;
#if OS_STK_GROWTH == 1u
#else
    pstk += size;
#endif

    nfree = 0u;
#if OS_STK_GROWTH == 1u
    while (*pstk++ == 0u) {                             //!< Compute the number of zero entries on the stk
        nfree++;
    }
#else
    while (*pstk-- == 0u) {
        nfree++;
    }
#endif
    ptcb->OSTCBStkUsed = size - nfree;                  //!< Compute number of entries used on the stk
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
#endif
