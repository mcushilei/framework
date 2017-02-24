
#include ".\os.h"

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)

/*********************************************************************************************************
*                                          LOCAL PROTOTYPES
*********************************************************************************************************/

static  void     OS_FlagTaskWait(OS_FLAG *pflag, OS_FLAG_NODE *pnode, INT32U timeout);
static  BOOLEAN  OS_FlagTaskRdy(OS_FLAG_NODE *pnode, INT8U pend_stat);

/*********************************************************************************************************
*                                        CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : flags         Contains the initial value to store in the event flag group.
*
* Returns    : OS_ERR_NONE               if the call was successful.
*              OS_ERR_CREATE_ISR         if you attempted to create an Event Flag from an ISR.
*              OS_ERR_FLAG_DEPLETED      if there are no more event flag control block
*
* Called from: Task ONLY
*********************************************************************************************************/

INT8U   osFlagCreate   (OS_HANDLE  *pFlagHandle,
                        BOOLEAN     init,
                        BOOLEAN     autoreset)
{
    OS_FLAG   **ppflag = (OS_FLAG **)pFlagHandle;
    OS_FLAG    *pflag;
#if OS_CRITICAL_METHOD == 3u                        //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif
    INT16U      flags = 0;


    if (OSIntNesting > 0u) {                        //!< See if called from ISR ...
        return OS_ERR_CREATE_ISR;                   //!< ... can't CREATE from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (ppflag == OS_NULL) {
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif

    if (init != 0u) {
        flags |= 0x01u;
    }
    if (autoreset != 0u) {
        flags |= 0x80u;
    }

    OS_ENTER_CRITICAL();
    pflag = OSFlagFreeList;                          //!< Get next free event flag
    *ppflag = pflag;
    if (pflag != OS_NULL) {                     //!< See if we have event flag available
        OSFlagFreeList        = (OS_FLAG *)OSFlagFreeList->OSFlagWaitList;//!< Adjust free list
        pflag->OSObjType      = OS_OBJ_TYPE_FLAG;  //!< Set to event flag group type
        pflag->OSFlagFlags    = flags;               //!< Set to desired initial value
        pflag->OSFlagWaitList = OS_NULL;             //!< Clear list of tasks waiting on flags
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    } else {
        OS_EXIT_CRITICAL();
        return OS_ERR_FLAG_DEPLETED;
    }
}

/*********************************************************************************************************
*                                     DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag
*              group.
*
* Arguments  : pflag          is a pointer to the desired event flag group.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Deletes the event flag group ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the event flag group even if tasks are
*                                                    waiting.  In this case, all the tasks pending will be
*                                                    readied.
*
* Returns    : OS_ERR_NONE              The call was successful and the event flag group was deleted
*              OS_ERR_DEL_ISR           If you attempted to delete the event flag group from an ISR
*              OS_ERR_FLAG_PFLAG_NULL   If 'pflag' is a NULL pointer.
*              OS_ERR_EVENT_TYPE        If you didn't pass a pointer to an event flag group
*              OS_ERR_INVALID_OPT       An invalid option was specified
*              OS_ERR_TASK_WAITING      One or more tasks were waiting on the event flag group.
*
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the event flag group MUST check the return code of OSFlagAccept() and osFlagPend().
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the event flag group.
*              3) All tasks that were waiting for the event flag will be readied and returned an
*                 OS_ERR_PEND_ABORT if osFlagDelete() was called with OS_DEL_ALWAYS
*********************************************************************************************************/

#if OS_FLAG_DEL_EN > 0u
INT8U   osFlagDelete    (OS_HANDLE   hFlag,
                         INT8U       opt)
{
    OS_FLAG      *pflag = (OS_FLAG *)hFlag;
    BOOLEAN       tasks_waiting;
    OS_FLAG_NODE *pnode;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR     cpu_sr = 0u;
#endif
    INT8U         err;


    if (OSIntNesting > 0u) {                               //!< See if called from ISR ...
        return OS_ERR_DEL_ISR;                             //!< ... can't DELETE from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (pflag == OS_NULL) {                            //!< Validate 'pflag'
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif
    if (pflag->OSObjType != OS_OBJ_TYPE_FLAG) {          //!< Validate event group type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (pflag->OSFlagWaitList != OS_NULL) {                 //!< See if any tasks waiting on event flags
        tasks_waiting = OS_TRUE;                            //!< Yes
    } else {
        tasks_waiting = OS_FALSE;                           //!< No
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                                //!< Delete if no task waiting
             if (tasks_waiting == OS_FALSE) {
                 pflag->OSObjType     = OS_OBJ_TYPE_UNUSED;
                 pflag->OSFlagWaitList = (void *)OSFlagFreeList;    //!< Return group to free list
                 pflag->OSFlagFlags    = 0u;
                 OSFlagFreeList        = pflag;
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_NONE;
             } else {
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_TASK_WAITING;
             }
             break;

        case OS_DEL_ALWAYS:                                 //!< Always delete the event flag
             pnode = (OS_FLAG_NODE *)pflag->OSFlagWaitList;
             while (pnode != OS_NULL) {                     //!< Ready ALL tasks waiting for flag
                 (void)OS_FlagTaskRdy(pnode, OS_STAT_PEND_ABORT);
                 pnode = (OS_FLAG_NODE *)pnode->OSFlagNodeNext;
             }
             pflag->OSObjType     = OS_OBJ_TYPE_UNUSED;
             pflag->OSFlagWaitList = (void *)OSFlagFreeList;//!< Return flag to free list
             pflag->OSFlagFlags    = 0u;
             OSFlagFreeList        = pflag;
             OS_EXIT_CRITICAL();
             if (tasks_waiting == OS_TRUE) {               //!< Reschedule only if task(s) were waiting
                 OS_Sched();                               //!< Find highest priority task ready to run
             }
             err = OS_ERR_NONE;
             break;

        default:
             OS_EXIT_CRITICAL();
             err = OS_ERR_INVALID_OPT;
             break;
    }
    return err;
}
#endif

/*********************************************************************************************************
*                                     WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag
*              group.  Your application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : pflag          is a pointer to the desired event flag group.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*  Returns    : OS_ERR_NONE               The desired bits have been set within the specified
*                                                      'timeout'.
*               OS_ERR_PEND_ISR           If you tried to PEND from an ISR
*               OS_ERR_PEND_LOCKED        If you called this function when the scheduler is locked
*               OS_ERR_FLAG_PFLAG_NULL    If 'pflag' is a NULL pointer.
*               OS_ERR_EVENT_TYPE         You are not pointing to an event flag group
*               OS_ERR_TIMEOUT            The bit(s) have not been set in the specified
*                                                      'timeout'.
*               OS_ERR_PEND_ABORT         The wait on the flag was aborted.
*
* Called from: Task ONLY
*
* Note(s)    : 1) IMPORTANT, the behavior of this function has changed from PREVIOUS versions.  The
*                 function NOW returns the flags that were ready INSTEAD of the current state of the
*                 event flags.
*********************************************************************************************************/

INT8U   osFlagPend  (OS_HANDLE       hFlag,
                     INT32U          timeout)
{
    OS_FLAG      *pflag = (OS_FLAG *)hFlag;
    OS_FLAG_NODE  node;
    INT16U        flags_rdy;
    INT16U        consume;
    INT8U         pend_stat;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR     cpu_sr = 0u;
#endif


    if (OSIntNesting > 0u) {                                //!< See if called from ISR ...
        return OS_ERR_PEND_ISR;                             //!< ... can't PEND from an ISR
    }
    if (OSLockNesting > 0u) {                               //!< See if called with scheduler locked ...
        return OS_ERR_PEND_LOCKED;                          //!< ... can't PEND when locked
    }
#if OS_ARG_CHK_EN > 0u
    if (pflag == OS_NULL) {                                 //!< Validate 'pflag'
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif
    if (pflag->OSObjType != OS_OBJ_TYPE_FLAG) {          //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    consume = (INT16U)(pflag->OSFlagFlags & 0x80u);
    flags_rdy = (INT16U)(pflag->OSFlagFlags & 0x01u);
    if (flags_rdy != 0u) {                          //!< See if flag set
        if (consume != 0u) {                        //!< Yes. See if we need to consume the flags
            pflag->OSFlagFlags &= (INT16U)~0x01u;           //!< Yes. Clear flag
        }
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    }

    if (timeout == 0u) {
        OS_EXIT_CRITICAL();
        return OS_ERR_TIMEOUT;
    }

    OS_FlagTaskWait(pflag, &node, timeout);                 //!< Suspend task until event occur or timeout
    OS_EXIT_CRITICAL();
    OS_Sched();                                             //!< Find next HPT ready to run

    OS_ENTER_CRITICAL();
    if (OSTCBCur->OSTCBStatPend != OS_STAT_PEND_OK) {       //!< Have we timed-out or aborted?
        pend_stat                = OSTCBCur->OSTCBStatPend;
        OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
        OSTCBCur->OSTCBStat      = OS_STAT_RDY;             //!< Yes, make task ready-to-run
        OS_FlagTaskRemove(&node);
        OS_EXIT_CRITICAL();
        switch (pend_stat) {
            case OS_STAT_PEND_ABORT:
                 return OS_ERR_PEND_ABORT;                  //!< Indicate that we aborted   waiting

            case OS_STAT_PEND_TO:
            default:
                 return OS_ERR_TIMEOUT;                     //!< Indicate that we timed-out waiting
        }
    }
    consume = (INT16U)(pflag->OSFlagFlags & 0x80u);         //!< Event must have occurred
    if (consume != 0u) {                            //!< See if we need to consume the flag
        pflag->OSFlagFlags &= (INT16U)~0x01u;               //!< Clear flag
    }
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}

/*********************************************************************************************************
*                                       SET EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to
*              set or clear are specified by a 'bit mask'.
*
* Arguments  : pflag          is a pointer to the desired event flag group.
*
* Returns    : OS_ERR_NONE                The call was successfull
*              OS_ERR_FLAG_PFLAG_NULL     You passed a NULL pointer
*              OS_ERR_EVENT_TYPE          You are not pointing to an event flag group
*
* Called From: Task or ISR
*
* WARNING(s) : 1) The execution time of this function depends on the number of tasks waiting on the event
*                 flag group.
*              2) The amount of time interrupts are DISABLED depends on the number of tasks waiting on
*                 the event flag group.
*********************************************************************************************************/

INT8U   osFlagSet   (OS_HANDLE  hFlag)
{
    OS_FLAG      *pflag = (OS_FLAG *)hFlag;
    OS_FLAG_NODE *pnode;
    BOOLEAN       sched;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR     cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pflag == OS_NULL) {                             //!< Validate 'pflag'
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif
    if (pflag->OSObjType != OS_OBJ_TYPE_FLAG) {      //!< Make sure we are pointing to an event flag
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    sched = OS_FALSE;                                       //!< Indicate that we don't need rescheduling
    if (pflag->OSFlagWaitList != OS_NULL) {
        sched = OS_TRUE;                                    //!< When done we will reschedule
        pnode = (OS_FLAG_NODE *)pflag->OSFlagWaitList;
        while (pnode != OS_NULL) {                          //!< Ready ALL tasks waiting for flag
            (void)OS_FlagTaskRdy(pnode, OS_STAT_PEND_OK);
            pnode = (OS_FLAG_NODE *)pnode->OSFlagNodeNext;  //!< Point to next task waiting for event flag
        }
    }
    if (sched == OS_TRUE) {                                 //!< Is there any task wait for this flag?
        if (pflag->OSFlagFlags & 0x80u) {                   //!< Yes. Is this a auto-reset flag?
            pflag->OSFlagFlags &= ~0x01u;                   //!<    Yes, Clear the flags
        } else {
            pflag->OSFlagFlags |=  0x01u;                   //!<    No, Set the flags
        }
    } else {
        pflag->OSFlagFlags |=  0x01u;                       //!< No. Set the flags.
    }
    OS_EXIT_CRITICAL();
    if (sched == OS_TRUE) {
        OS_Sched();
    }
    return OS_ERR_NONE;
}

/*********************************************************************************************************
*                                       RESET EVENT FLAG BIT(S)
*
* Description: This function is called to reset or clear some bits in an event flag group.  The bits to
*              set or clear are specified by a 'bit mask'.
*
* Arguments  : pflag          is a pointer to the desired event flag group.
*
* Returns    : OS_ERR_NONE                The call was successfull
*              OS_ERR_FLAG_PFLAG_NULL     You passed a NULL pointer
*              OS_ERR_EVENT_TYPE          You are not pointing to an event flag group
*
* Called From: Task or ISR
*
* WARNING(s) : 1) The execution time of this function depends on the number of tasks waiting on the event
*                 flag group.
*              2) The amount of time interrupts are DISABLED depends on the number of tasks waiting on
*                 the event flag group.
*********************************************************************************************************/

INT8U   osFlagReset (OS_HANDLE  hFlag)
{
    OS_FLAG    *pflag = (OS_FLAG *)hFlag;
#if OS_CRITICAL_METHOD == 3u                         //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pflag == OS_NULL) {                  //!< Validate 'pflag'
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif
    if (pflag->OSObjType != OS_OBJ_TYPE_FLAG) {    //!< Make sure we are pointing to an event flag grp
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    pflag->OSFlagFlags &= (INT16U)~0x01u;             //!< Reset the flags
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
/*********************************************************************************************************
*                                          QUERY EVENT FLAG
*
* Description: This function is used to check the value of the event flag group.
*
* Arguments  : pflag         is a pointer to the desired event flag group.
*
* Returns    : OS_ERR_NONE                The call was successfull
*              OS_ERR_FLAG_PFLAG_NULL     You passed a NULL pointer
*              OS_ERR_EVENT_TYPE          You are not pointing to an event flag group
*
* Called From: Task or ISR
*********************************************************************************************************/

#if OS_FLAG_QUERY_EN > 0u
INT8U   osFlagQuery (OS_HANDLE      hFlag,
                     OS_FLAG_DATA  *p_flag_data)
{
    OS_FLAG    *pflag = (OS_FLAG *)hFlag;
    INT16U      flag;
#if OS_CRITICAL_METHOD == 3u                      //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if ((pflag == OS_NULL) || (p_flag_data == OS_NULL)) { //!< Validate 'pflag', 'pflag'
        return OS_ERR_FLAG_PFLAG_NULL;
    }
#endif
    if (pflag->OSObjType != OS_OBJ_TYPE_FLAG) { //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    flag = pflag->OSFlagFlags;
    if (flag & 0x80 != 0) {
        p_flag_data->OSFlagAutoReset = OS_TRUE;
    } else {
        p_flag_data->OSFlagAutoReset = OS_FALSE;
    }
    if (flag & 0x01 != 0) {
        p_flag_data->OSFlagStatus = OS_TRUE;
    } else {
        p_flag_data->OSFlagStatus = OS_FALSE;
    }
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
#endif

/*********************************************************************************************************
*                     SUSPEND TASK UNTIL EVENT FLAG(s) RECEIVED OR TIMEOUT OCCURS
*
* Description: This function is internal to OS and is used to put a task to sleep until the desired
*              event flag bit(s) are set.
*
* Arguments  : pflag         is a pointer to the desired event flag.
*
*              pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
*              timeout       is the desired amount of time that the task will wait for the event flag
*                            bit(s) to be set.
*
* Returns    : none
*
* Called by  : osFlagPend()  os_flag.c
*
* Note(s)    : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

static void OS_FlagTaskWait(OS_FLAG        *pflag,
                            OS_FLAG_NODE   *pnode,
                            INT32U          timeout)
{
    OS_FLAG_NODE  *pNodeNext;
    INT8U          y;


    pnode->OSFlagNodeTCB      = (void *)OSTCBCur;       //!< Link to task's TCB
    pnode->OSFlagNodeNext     = pflag->OSFlagWaitList;  //!< Add node at beginning of event flag wait list
    pnode->OSFlagNodePrev     = OS_NULL;
    pnode->OSFlagNodeFlag     = (void *)pflag;          //!< Link to Flag

    pNodeNext = (OS_FLAG_NODE *)pflag->OSFlagWaitList; //!< Adjust flag's wait list
    if (pNodeNext != OS_NULL) {                        //!< Is this the first NODE to insert?
        pNodeNext->OSFlagNodePrev = pnode;             //!< No, link in doubly linked list
    }
    pflag->OSFlagWaitList = (void *)pnode;

    OSTCBCur->OSTCBStat      |= OS_STAT_FLAG;
    OSTCBCur->OSTCBStatPend   = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly        = timeout;                //!< Store timeout in task's TCB
    OSTCBCur->OSTCBFlagNode   = pnode;                  //!< Store node in task's TCB

    y            =  OSTCBCur->OSTCBY;                   //!< Suspend current task
    OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
    if (OSRdyTbl[y] == 0u) {
        OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
    }
}

/*********************************************************************************************************
*                              MAKE TASK READY-TO-RUN, EVENT(s) OCCURRED
*
* Description: This function is internal to OS and is used to make a task ready-to-run because the
*              desired event flag bits have been set.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
*              pend_stat   is used to indicate the readied task's pending status:
*
*
* Returns    : OS_TRUE       If the task has been placed in the ready list and thus needs scheduling
*              OS_FALSE      The task is still not ready to run and thus scheduling is not necessary
*
* Called by  : OSFlagsPost() os_flag.c
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

static BOOLEAN OS_FlagTaskRdy  (OS_FLAG_NODE   *pnode,
                                INT8U           pend_stat)
{
    OS_TCB   *ptcb;
    INT8U     y;


    ptcb                 = (OS_TCB *)pnode->OSFlagNodeTCB;  //!< Point to TCB of waiting task
    ptcb->OSTCBDly       = 0u;
    ptcb->OSTCBStat     &= (INT8U)~OS_STAT_FLAG;
    ptcb->OSTCBStatPend  = pend_stat;

    y            = ptcb->OSTCBY;
    OSRdyGrp    |= ptcb->OSTCBBitY;              //!< Put task into ready table
    OSRdyTbl[y] |= ptcb->OSTCBBitX;

    OS_FlagTaskRemove(pnode);
    return OS_TRUE;
}

/*********************************************************************************************************
*                              UNLINK EVENT FLAG NODE FROM WAITING LIST
*
* Description: This function is internal to OS and is used to unlink an event flag node from a
*              list of tasks waiting for the event flag.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for
*                            event flag bit(s) to be set.
*
* Returns    : none
*
* Called by  : OS_FlagTaskRdy() os_flag.c
*              osFlagPend()     os_flag.c
*              OS_TaskDel()      os_task.c
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

void    OS_FlagTaskRemove  (OS_FLAG_NODE *pnode)
{
    OS_TCB       *ptcb;
    OS_FLAG      *pflag;
    OS_FLAG_NODE *pNodePrev;
    OS_FLAG_NODE *pNodeNext;


    pNodePrev = (OS_FLAG_NODE *)pnode->OSFlagNodePrev;
    pNodeNext = (OS_FLAG_NODE *)pnode->OSFlagNodeNext;
    if (pNodePrev == OS_NULL) {                                     //!< Is it first node in wait list?
        pflag                 = (OS_FLAG *)pnode->OSFlagNodeFlag;   //!< Yes...
        pflag->OSFlagWaitList = (void *)pNodeNext;                  //!< Update list for new 1st node
        if (pNodeNext != OS_NULL) {
            pNodeNext->OSFlagNodePrev = OS_NULL;                    //!< Link new 1st node PREV to NULL
        }
    } else {                                                        //!< No, A node somewhere in the list
        pNodePrev->OSFlagNodeNext = pNodeNext;                      //!< Link around the node to unlink
        if (pNodeNext != OS_NULL) {                                 //!< Was this the LAST node?
            pNodeNext->OSFlagNodePrev = pNodePrev;                  //!< No, Link around current node
        }
    }
    ptcb = (OS_TCB *)pnode->OSFlagNodeTCB;
    ptcb->OSTCBFlagNode = OS_NULL;
}

/*********************************************************************************************************
*                                  INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by OS to initialize the event flag module.  Your application
*              MUST NOT call this function.  In other words, this function is internal to OS.
*
* Arguments  : none
*
* Returns    : none
*
* WARNING    : You MUST NOT call this function from your code.  This is an INTERNAL function to OS.
*********************************************************************************************************/

void  OS_FlagListInit (void)
{
    uint_fast16_t   i;
    OS_FLAG        *pflag;
    OS_FLAG       **ppflag;


    OS_MemClr((INT8U *)&OSFlagFreeTbl[0], sizeof(OSFlagFreeTbl));       //!< Clear the flag group table

    ppflag = &OSFlagFreeList;
    for (i = 0u; i < (sizeof(OSFlagFreeTbl) / sizeof(OS_FLAG)); i++) {  //!< Init. list of free EVENT FLAGS
        pflag   = &OSFlagFreeTbl[i];
        *ppflag = pflag;
        ppflag  = (OS_FLAG **)&pflag->OSFlagWaitList;
    }
}


#endif      //!< (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
