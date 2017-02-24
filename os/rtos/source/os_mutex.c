
#include ".\os.h"

#if OS_MUTEX_EN > 0u

/*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************/

#define  OS_MUTEX_KEEP_LOWER_8   ((INT16U)0x00FFu)
#define  OS_MUTEX_KEEP_UPPER_8   ((INT16U)0xFF00u)

#define  OS_MUTEX_AVAILABLE      ((INT16U)0x00FFu)

/*********************************************************************************************************
*                                 CREATE A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function creates a mutual exclusion semaphore.
*
* Returns    : OS_ERR_NONE              If the call was successful.
*              OS_ERR_CREATE_ISR        If you attempted to create a MUTEX from an ISR
*              OS_ERR_PEVENT_NULL       If 'ppevent' is a NULL pointer.
*              OS_ERR_EVENT_DEPLETED    No more event control blocks available.
*
* Note(s)    : 1) The LEAST significant 8 bits of '.OSEventCnt' hold the priority number of the task
*                 owning the mutex or 0xFF if no task owns the mutex.
*********************************************************************************************************/

INT8U   osMutexCreate (OS_HANDLE   *pMutexHandle)
{
    OS_EVENT  **ppevent = (OS_EVENT **)pMutexHandle;
    OS_EVENT   *pevent;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    if (OSIntNesting > 0u) {                               //!< See if called from ISR ...
        return OS_ERR_CREATE_ISR;                          //!< ... can't CREATE mutex from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (ppevent == OS_NULL) {                       //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif

    OS_ENTER_CRITICAL();

    pevent = OSEventFreeList;                               //!< Get next free event control block
    if (pevent == OS_NULL) {                                //!< See if an ECB was available
        OS_EXIT_CRITICAL();
        return OS_ERR_EVENT_DEPLETED;                       //!< No more event control blocks
    }
    OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr; //!< Adjust the free list
    OS_EXIT_CRITICAL();

    pevent->OSObjType   = OS_OBJ_TYPE_MUTEX;
    pevent->OSEventCnt  = OS_MUTEX_AVAILABLE;               //!< Resource is avail.
    pevent->OSEventPtr  = OS_NULL;                          //!< No task owning the mutex
    OS_EventWaitTableInit(pevent);                          //!< Initialize to 'nobody waiting' on mut.
    *ppevent = pevent;
    return OS_ERR_NONE;
}

/*********************************************************************************************************
*                                           DELETE A MUTEX
*
* Description: This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mutex.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete mutex ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the mutex even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
* Returns    : OS_ERR_NONE             The call was successful and the mutex was deleted
*              OS_ERR_DEL_ISR          If you attempted to delete the MUTEX from an ISR
*              OS_ERR_INVALID_OPT      An invalid option was specified
*              OS_ERR_TASK_WAITING     One or more tasks were waiting on the mutex
*              OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*              OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mutex MUST check the return code of osMutexPend().
*
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mutex.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful because the
*                 resource(s) will no longer be guarded by the mutex.
*
*              4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that the owner of the Mutex (if there
*                            is one) is ready-to-run and is thus NOT pending on another kernel object or
*                            has delayed itself.  In other words, if a task owns the mutex being deleted,
*                            that task will be made ready-to-run at its original priority.
*********************************************************************************************************/

#if OS_MUTEX_DEL_EN > 0u
INT8U   osMutexDelete   (OS_HANDLE  hMutex,
                         INT8U      opt)
{
    OS_EVENT  *pevent = (OS_EVENT *)hMutex;
    BOOLEAN    tasks_waiting;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      err;


    if (OSIntNesting > 0u) {                               //!< See if called from ISR ...
        return OS_ERR_DEL_ISR;                             //!< ... can't DELETE from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                         //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_MUTEX) {      //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u) {                        //!< See if any tasks waiting on mutex
        tasks_waiting = OS_TRUE;                           //!< Yes
    } else {
        tasks_waiting = OS_FALSE;                          //!< No
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                               //!< DELETE MUTEX ONLY IF NO TASK WAITING ---
             if (tasks_waiting == OS_FALSE) {
                 pevent->OSObjType   = OS_OBJ_TYPE_UNUSED;
                 pevent->OSEventPtr    = OSEventFreeList;  //!< Return Event Control Block to free list
                 pevent->OSEventCnt    = 0u;
                 OSEventFreeList       = pevent;
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_NONE;
             } else {
                 OS_EXIT_CRITICAL();
                 err = OS_ERR_TASK_WAITING;
             }
             break;

        case OS_DEL_ALWAYS:                                //!< ALWAYS DELETE THE MUTEX ----------------
             while (pevent->OSEventGrp != 0u) {            //!< Ready ALL tasks waiting for mutex
                 (void)OS_EventTaskRdy(pevent, OS_STAT_MUTEX, OS_STAT_PEND_ABORT);
             }
             pevent->OSObjType   = OS_OBJ_TYPE_UNUSED;
             pevent->OSEventPtr    = OSEventFreeList;      //!< Return Event Control Block to free list
             pevent->OSEventCnt    = 0u;
             OSEventFreeList       = pevent;               //!< Get next free event control block
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
*                                 PEND ON MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function waits for a mutual exclusion semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            mutex.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            mutex or, until the resource becomes available.
*
* Returns    : OS_ERR_NONE        The call was successful and your task owns the mutex
*              OS_ERR_TIMEOUT     The mutex was not available within the specified 'timeout'.
*              OS_ERR_PEND_ABORT  The wait on the mutex was aborted.
*              OS_ERR_PEND_ISR    If you called this function from an ISR and the result
*                                 would lead to a suspension.
*              OS_ERR_PEND_LOCKED If you called this function when the scheduler is locked
*              OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a mutex
*              OS_ERR_PEVENT_NULL 'pevent' is a NULL pointer
*
* Note(s)    : 1) The task that owns the mutex MUST NOT pend on any other event while it owns the mutex.
*
*              2) You MUST NOT change the priority of the task that owns the mutex
*********************************************************************************************************/

INT8U   osMutexPend (OS_HANDLE  hMutex,
                     INT32U     timeout)
{
    OS_EVENT  *pevent = (OS_EVENT *)hMutex;
#if OS_CRITICAL_METHOD == 3u                                //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif
    INT8U      err;


    if (OSIntNesting > 0u) {                                //!< See if called from ISR ...
        return OS_ERR_PEND_ISR;                             //!< ... can't PEND from an ISR
    }
    if (OSLockNesting > 0u) {                               //!< See if called with scheduler locked ...
        return OS_ERR_PEND_LOCKED;                          //!< ... can't PEND when locked
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                                //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_MUTEX) {       //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
                                                            //!< Is Mutex available?
    if ((INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8) == OS_MUTEX_AVAILABLE) {
        pevent->OSEventCnt  = (INT16U)OSTCBCur->OSTCBPrio;  //!<      Save priority of owning task
        pevent->OSEventPtr  = (void *)OSTCBCur;             //!<      Point to owning task's OS_TCB
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    }
                                                            //!< Mutex not available
    if (timeout == 0u) {
        OS_EXIT_CRITICAL();
        return OS_ERR_TIMEOUT;
    }

    OSTCBCur->OSTCBStat     |= OS_STAT_MUTEX;               //!< Pend current task
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;                     //!< Store timeout in current task's TCB
    OS_EventTaskWait(pevent);                               //!< Suspend task until event or timeout occurs
    OS_EXIT_CRITICAL();
    OS_Sched();                                             //!< Find next highest priority task ready

    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend) {                      //!< See if we timed-out or aborted
        case OS_STAT_PEND_OK:
             err = OS_ERR_NONE;
             break;

        case OS_STAT_PEND_ABORT:
             err = OS_ERR_PEND_ABORT;                       //!< Indicate that we aborted getting mutex
             break;

        case OS_STAT_PEND_TO:
        default:
             OS_EventTaskRemove(OSTCBCur, pevent);
             err = OS_ERR_TIMEOUT;                          //!< Indicate that we didn't get mutex within TO
             break;
    }
    OSTCBCur->OSTCBStat          = OS_STAT_RDY;             //!< Set   task  status to ready
    OSTCBCur->OSTCBStatPend      = OS_STAT_PEND_OK;         //!< Clear pend  status
    OSTCBCur->OSTCBEventPtr      = OS_NULL;                 //!< Clear event pointers
    OS_EXIT_CRITICAL();
    return err;
}

/*********************************************************************************************************
*                                POST TO A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function signals a mutual exclusion semaphore
*
* Arguments  : pevent              is a pointer to the event control block associated with the desired
*                                  mutex.
*
* Returns    : OS_ERR_NONE             The call was successful and the mutex was signaled.
*              OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*              OS_ERR_PEVENT_NULL      'pevent' is a NULL pointer
*              OS_ERR_POST_ISR         Attempted to post from an ISR (not valid for MUTEXes)
*              OS_ERR_NOT_MUTEX_OWNER  The task that did the post is NOT the owner of the MUTEX.
*
* Note(s)    : 1) The mutex can ONLY be released by it's owner.
*********************************************************************************************************/

INT8U   osMutexPost (OS_HANDLE  hMutex)
{
    OS_EVENT  *pevent = (OS_EVENT *)hMutex;
    INT8U      prio;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (OSIntNesting > 0u) {                            //!< See if called from ISR ...
        return OS_ERR_POST_ISR;                         //!< ... can't POST mutex from an ISR
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                            //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_MUTEX) {   //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    if (OSTCBCur != (OS_TCB *)pevent->OSEventPtr) {     //!< See if posting task owns the MUTEX
        OS_EXIT_CRITICAL();
        return OS_ERR_NOT_MUTEX_OWNER;
    }
    if (pevent->OSEventGrp != 0u) {                     //!< Any task waiting for the mutex?
                                                        //!< Yes, Make HPT waiting for mutex ready
        prio                = OS_EventTaskRdy(pevent, OS_STAT_MUTEX, OS_STAT_PEND_OK);
        pevent->OSEventCnt  = (INT16U)prio;
        pevent->OSEventPtr  = OSTaskPrioTCBTbl[prio]; //!<      Link to new mutex owner's OS_TCB
        OS_EXIT_CRITICAL();
        OS_Sched();                                     //!<      Find highest priority task ready to run
        return OS_ERR_NONE;
    } else {
        pevent->OSEventCnt  = OS_MUTEX_AVAILABLE;       //!< No,  Mutex is now available
        pevent->OSEventPtr  = OS_NULL;
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    }
}

/*********************************************************************************************************
*                                 QUERY A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function obtains information about a mutex
*
* Arguments  : pevent          is a pointer to the event control block associated with the desired mutex
*
*              p_mutex_data    is a pointer to a structure that will contain information about the mutex
*
* Returns    : OS_ERR_NONE          The call was successful and the message was sent
*              OS_ERR_PEVENT_NULL   If 'pevent'       is a NULL pointer
*              OS_ERR_PDATA_NULL    If 'p_mutex_data' is a NULL pointer
*              OS_ERR_EVENT_TYPE    If you are attempting to obtain data from a non mutex.
*********************************************************************************************************/

#if OS_MUTEX_QUERY_EN > 0u
INT8U  osMutexQuery (OS_HANDLE      hMutex,
                     OS_MUTEX_DATA *p_mutex_data)
{
    OS_EVENT       *pevent = (OS_EVENT *)hMutex;
    OS_PRIO        *psrc;
    OS_PRIO        *pdest;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif
    uint_fast8_t    i;


#if OS_ARG_CHK_EN > 0u
    if (pevent == OS_NULL) {                            //!< Validate 'pevent'
        return OS_ERR_PEVENT_NULL;
    }
    if (p_mutex_data == OS_NULL) {                      //!< Validate 'p_mutex_data'
        return OS_ERR_PDATA_NULL;
    }
#endif
    if (pevent->OSObjType != OS_OBJ_TYPE_MUTEX) {   //!< Validate event block type
        return OS_ERR_EVENT_TYPE;
    }

    OS_ENTER_CRITICAL();
    p_mutex_data->OSEventGrp  = pevent->OSEventGrp;     //!< Copy wait list
    psrc                      = &pevent->OSEventTbl[0];
    pdest                     = &p_mutex_data->OSEventTbl[0];
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;
    }
    p_mutex_data->OSOwnerPrio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);    //!< Get owner's priority
    if (p_mutex_data->OSOwnerPrio == OS_MUTEX_AVAILABLE) {
        p_mutex_data->OSValue = OS_TRUE;
    } else {
        p_mutex_data->OSValue = OS_FALSE;
    }
    OS_EXIT_CRITICAL();
    return OS_ERR_NONE;
}
#endif      //!< OS_MUTEX_QUERY_EN

#endif      //!< OS_MUTEX_EN
