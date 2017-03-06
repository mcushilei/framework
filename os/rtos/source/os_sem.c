
#include ".\os.h"

#if OS_SEM_EN > 0u

/*********************************************************************************************************
*                                         CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : ppevent       is a pointer to a pointer of the event control block
*
*              cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : OS_ERR_NONE              If the call was successful.
*              OS_ERR_CREATE_ISR        If you attempted to create a MUTEX from an ISR
*              OS_ERR_PEVENT_NULL       If 'ppevent' is a NULL pointer.
*              OS_ERR_EVENT_DEPLETED    No more event control blocks available.
*********************************************************************************************************/

OS_ERR   osSemCreate (OS_HANDLE  *pSemaphoreHandle,
                     INT16U      cnt)
{
    OS_EVENT  **ppevent = (OS_EVENT **)pSemaphoreHandle;
    OS_EVENT   *pevent;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    if (OSIntNesting > 0u) {                               //!< See if called from ISR ...
        return OS_ERR_CREATE_ISR;                          //!< ... can't CREATE from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (ppevent == (OS_EVENT **)0) {                       //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif

    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                               //!< Get next free event control block
    if (pevent == OS_NULL) {                                //!< See if pool of free ECB pool was empty
        OS_EXIT_CRITICAL();
        return OS_ERR_EVENT_DEPLETED;                       //!< No more event control blocks
    }
    OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr; //!< Adjust the free list
    OS_EXIT_CRITICAL();
    
    pevent->OSObjType   = OS_OBJ_TYPE_SEM;
    pevent->OSEventCnt  = cnt;                           //!< Set semaphore value
    pevent->OSEventPtr  = OS_NULL;                       //!< Unlink from ECB free list
    OS_EventWaitTableInit(pevent);                          //!< Initialize to 'nobody waiting' on sem.
    *ppevent = pevent;
    return OS_ERR_NONE;
}

/*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete semaphore ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
* Returns    : OS_ERR_NONE             The call was successful and the semaphore was deleted
*              OS_ERR_DEL_ISR          If you attempted to delete the semaphore from an ISR
*              OS_ERR_INVALID_OPT      An invalid option was specified
*              OS_ERR_TASK_WAITING     One or more tasks were waiting on the semaphore
*              OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a semaphore
*              OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the semaphore MUST check the return code of osSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the semaphore.
*              4) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in
*                 applications where the semaphore is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the semaphore.
*              5) All tasks that were waiting for the semaphore will be readied and returned an
*                 OS_ERR_PEND_ABORT if osSemDelete() was called with OS_DEL_ALWAYS
*********************************************************************************************************/

#if OS_SEM_DEL_EN > 0u
OS_ERR   osSemDelete (OS_HANDLE  hSemaphore,
                     INT8U      opt)
{
    OS_EVENT  *pevent = (OS_EVENT *)hSemaphore;
    BOOLEAN    tasks_waiting;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      err;


    if (OSIntNesting > 0u) {                               //!< See if called from ISR ...
        return OS_ERR_DEL_ISR;                            //!< ... can't DELETE from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                         //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {        //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        //!< See if any tasks waiting on semaphore
        tasks_waiting = OS_TRUE;                           //!< Yes
    } else {
        tasks_waiting = OS_FALSE;                          //!< No
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                               //!< Delete semaphore only if no task waiting
             if (tasks_waiting == OS_FALSE) {
                 pevent->OSObjType    = OS_OBJ_TYPE_UNUSED;
                 pevent->OSEventPtr     = OSEventFreeList; //!< Return Event Control Block to free list
                 pevent->OSEventCnt     = 0u;
                 OSEventFreeList        = pevent;          //!< Get next free event control block
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_NONE;
             } else {
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_TASK_WAITING;
             }
             break;

        case OS_DEL_ALWAYS:                                //!< Always delete the semaphore
             while (pevent->OSEventGrp != 0u) {            //!< Ready ALL tasks waiting for semaphore
                 (void)OS_EventTaskRdy(pevent, OS_STAT_SEM, OS_STAT_PEND_ABORT);
             }
             pevent->OSObjType    = OS_OBJ_TYPE_UNUSED;
             pevent->OSEventPtr     = OSEventFreeList;     //!< Return Event Control Block to free list
             pevent->OSEventCnt     = 0u;
             OSEventFreeList        = pevent;              //!< Get next free event control block
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
*                                          PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
* Returns    : OS_ERR_NONE         The call was successful and your task owns the resource
*                                  or, the event you are waiting for occurred.
*              OS_ERR_TIMEOUT      The semaphore was not received within the specified
*                                  'timeout'.
*              OS_ERR_PEND_ABORT   The wait on the semaphore was aborted.
*              OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*              OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*              OS_ERR_PEND_LOCKED  If you called this function when the scheduler is locked
*                                  would lead to a suspension.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*********************************************************************************************************/

OS_ERR   osSemPend (OS_HANDLE  hSemaphore,
                   INT32U     timeout)
{
    OS_EVENT  *pevent = (OS_EVENT *)hSemaphore;
#if OS_CRITICAL_METHOD == 3u                          //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      err;


    if (OSIntNesting > 0u) {                          //!< See if called from ISR ...
        return OS_ERR_PEND_ISR;                      //!< ... can't PEND from an ISR
    }
    if (OSLockNesting > 0u) {                         //!< See if called with scheduler locked ...
        return OS_ERR_PEND_LOCKED;                   //!< ... can't PEND when locked
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                    //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {   //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (pevent->OSEventCnt > 0u) {                    //!< If sem. is positive, resource available ...
        pevent->OSEventCnt--;                         //!< ... decrement semaphore only if positive.
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    }

    if (timeout == 0u) {
        OS_EXIT_CRITICAL();
        return OS_ERR_TIMEOUT;
    }
                                                      //!< Otherwise, must wait until event occurs
    OSTCBCur->OSTCBStat     |= OS_STAT_SEM;           //!< Resource not available, pend on semaphore
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;               //!< Store pend timeout in TCB
    OS_EventTaskWait(pevent);                         //!< Suspend task until event or timeout occurs
    OS_EXIT_CRITICAL();
    OS_Sched();                                       //!< Find next highest priority task ready

    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend) {                //!< See if we timed-out or aborted
        case OS_STAT_PEND_OK:
             err = OS_ERR_NONE;
             break;

        case OS_STAT_PEND_ABORT:
             err = OS_ERR_PEND_ABORT;               //!< Indicate that we aborted
             break;

        case OS_STAT_PEND_TO:
        default:
             OS_EventTaskRemove(OSTCBCur, pevent);
             err = OS_ERR_TIMEOUT;                  //!< Indicate that we didn't get event within TO
             break;
    }
    OSTCBCur->OSTCBStat          = OS_STAT_RDY;     //!< Set   task  status to ready
    OSTCBCur->OSTCBStatPend      = OS_STAT_PEND_OK; //!< Clear pend  status
    OSTCBCur->OSTCBEventPtr      = OS_NULL;         //!< Clear event pointers
    OS_EXIT_CRITICAL();
    return err;
}

/*********************************************************************************************************
*                                    ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function
*              should be used to fault-abort the wait on the semaphore, rather than to normally signal
*              the semaphore via osSemPost().
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines the type of ABORT performed:
*                            OS_PEND_OPT_NONE         ABORT wait for the highest priority task (HPT) that
*                                                     is waiting on the semaphore
*                            OS_PEND_OPT_BROADCAST    ABORT wait for ALL tasks that are  waiting on the
*                                                     semaphore
*
* Returns    : OS_ERR_NONE         No tasks were     waiting on the semaphore.
*              OS_ERR_PEND_ABORT   At least one task waiting on the semaphore was readied
*                                  and informed of the aborted wait; check return value
*                                  for the number of tasks whose wait on the semaphore
*                                  was aborted.
*              OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*********************************************************************************************************/

#if OS_SEM_PEND_ABORT_EN > 0u
OS_ERR  osSemPendAbort (OS_HANDLE  hSemaphore,
                       INT8U      opt)
{
    OS_EVENT  *pevent = (OS_EVENT *)hSemaphore;
    INT8U      nbr_tasks;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                            //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {     //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                     //!< See if any task waiting on semaphore?
        nbr_tasks = 0u;
        switch (opt) {
            case OS_PEND_OPT_BROADCAST:                 //!< Do we need to abort ALL waiting tasks?
                 while (pevent->OSEventGrp != 0u) {     //!< Yes, ready ALL tasks waiting on semaphore
                     (void)OS_EventTaskRdy(pevent, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                     nbr_tasks++;
                 }
                 break;

            case OS_PEND_OPT_NONE:
            default:                                    //!< No,  ready HPT       waiting on semaphore
                 (void)OS_EventTaskRdy(pevent, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                 nbr_tasks++;
                 break;
        }
        OS_EXIT_CRITICAL();
        OS_Sched();                                     //!< Find HPT ready to run
        return OS_ERR_PEND_ABORT;
    } else {
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    }
}
#endif

/*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              cnt           is the value for the semaphore count to add.
*
* Returns    : OS_ERR_NONE         The call was successful and the semaphore was signaled.
*              OS_ERR_SEM_OVF      If the semaphore count exceeded its limit. In other words, you have
*                                  signaled the semaphore more often than you waited on it with either
*                                  OSSemAccept() or osSemPend().
*              OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*********************************************************************************************************/

OS_ERR  osSemPost (OS_HANDLE hSemaphore,
                  INT16U    cnt)
{
    OS_EVENT  *pevent = (OS_EVENT *)hSemaphore;
    INT8U       err;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                            //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {     //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (cnt <= (65535u - pevent->OSEventCnt)) {          //!< Make sure semaphore will not overflow
        pevent->OSEventCnt += cnt;                      //!< Increment semaphore count to register event
        err = OS_ERR_NONE;
    } else {
        err = OS_ERR_SEM_OVF;
    }
    if (pevent->OSEventGrp != 0u) {                     //!< See if any task waiting for semaphore
                                                        //!< Ready HPT waiting on event
        (void)OS_EventTaskRdy(pevent, OS_STAT_SEM, OS_STAT_PEND_OK);
        OS_EXIT_CRITICAL();
        OS_Sched();                                     //!< Find HPT ready to run
        return err;
    } else {
        OS_EXIT_CRITICAL();
        return err;
    }
}

/*********************************************************************************************************
*                                            SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically,
*              this value would be 0.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : pevent     is a pointer to the event control block
*
*              cnt        is the new value for the semaphore count.  You would pass 0 to reset the
*                         semaphore count.
*
* Returns    : OS_ERR_NONE          The call was successful and the semaphore value was set.
*              OS_ERR_EVENT_TYPE    If you didn't pass a pointer to a semaphore.
*              OS_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer.
*              OS_ERR_TASK_WAITING  If tasks are waiting on the semaphore.
*********************************************************************************************************/

#if OS_SEM_SET_EN > 0u
OS_ERR   osSemSet    (OS_HANDLE  hSemaphore,
                     INT16U     cnt)
{
    OS_EVENT  *pevent = (OS_EVENT *)hSemaphore;
#if OS_CRITICAL_METHOD == 3u                          //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      err;


#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                    //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {   //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    err = OS_ERR_NONE;
    if (pevent->OSEventCnt > 0u) {                    //!< See if semaphore already has a count
        pevent->OSEventCnt = cnt;                     //!< Yes, set it to the new value specified.
    } else {                                          //!< No
        if (pevent->OSEventGrp == 0u) {               //!<      See if task(s) waiting?
            pevent->OSEventCnt = cnt;                 //!<      No, OK to set the value
        } else {
            err = OS_ERR_TASK_WAITING;
        }
    }
    OS_EXIT_CRITICAL();
    return err;
}
#endif

/*********************************************************************************************************
*                                          QUERY A SEMAPHORE
*
* Description: This function obtains information about a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore
*
*              p_sem_data    is a pointer to a structure that will contain information about the
*                            semaphore.
*
* Returns    : OS_ERR_NONE         The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non semaphore.
*              OS_ERR_PEVENT_NULL  If 'pevent'     is a NULL pointer.
*              OS_ERR_PDATA_NULL   If 'p_sem_data' is a NULL pointer
*********************************************************************************************************/

#if OS_SEM_QUERY_EN > 0u
OS_ERR  osSemQuery (OS_HANDLE    hSemaphore,
                   OS_SEM_DATA *p_sem_data)
{
    OS_EVENT       *pevent = (OS_EVENT *)hSemaphore;
    OS_PRIO        *psrc;
    OS_PRIO        *pdest;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif
    uint_fast8_t    i;


#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                         //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
    if (p_sem_data == OS_NULL) {                  //!< Validate 'p_sem_data'
        return OS_ERR_PDATA_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_SEM) {        //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    p_sem_data->OSEventGrp = pevent->OSEventGrp;           //!< Copy wait list
    psrc                   = &pevent->OSEventTbl[0];
    pdest                  = &p_sem_data->OSEventTbl[0];
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;
    }
    p_sem_data->OSCnt = pevent->OSEventCnt;                //!< Get semaphore count
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
#endif      //!< OS_SEM_QUERY_EN

#endif      //!< OS_SEM_EN
