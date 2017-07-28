/*******************************************************************************
 *  Copyright(C)2017 by Dreistein<mcu_shilei@hotmail.com>                     *
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


//! \note do not move this pre-processor statement to other places
#define  __OS_MUTEX_C__

/*============================ INCLUDES ======================================*/
#include ".\os.h"

#if (OS_MUTEX_EN > 0u) && (OS_MAX_MUTEXES >0u)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

/*!
 *! \Brief       CREATE A MUTEX
 *!
 *! \Description This function creates a mutex.
 *!
 *! \Arguments   pMutexHandle  is a pointer to the handle to the event control block associated
 *!                            with the desired mutex.
 *!
 *!              ceilingPrio   ceiling priority of the mutex. You can disable this function of
 *!                            the mutex by setting this value to OS_TASK_LOWEST_PRIO.
 *!
 *! \Returns     OS_ERR_NONE            If the call was successful.
 *!              OS_ERR_USE_IN_ISR      If you attempted to create a MUTEX from an ISR
 *!              OS_ERR_INVALID_HANDLE  If 'ppevent' is a NULL pointer.
 *!              OS_ERR_OBJ_DEPLETED    No more event control blocks available.
 *!              OS_ERR_TASK_EXIST      The ceiling priority has been used by task.
 */
OS_ERR osMutexCreate(OS_HANDLE *pMutexHandle, UINT8 ceilingPrio)
{
    OS_MUTEX   *pmutex;
#if OS_CRITICAL_METHOD == 3u            //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pMutexHandle == NULL) {         //!< Validate handle
        return OS_ERR_INVALID_HANDLE;
    }
#if OS_MAX_PRIO_LEVELS <= 255
    if (ceilingPrio >= OS_MAX_PRIO_LEVELS) {    //!< Make sure priority is within allowable range
        return OS_ERR_INVALID_PRIO;
    }
#endif
#endif
    if (osIntNesting > 0u) {            //!< See if called from ISR ...
        return OS_ERR_USE_IN_ISR;       //!< ... can't CREATE mutex from an ISR
    }

    //! malloc an ECB from pool.
    OSEnterCriticalSection(cpu_sr);
    pmutex = OS_ObjPoolNew(&osMutexFreeList);
    if (pmutex == NULL) {
        OSExitCriticalSection(cpu_sr);
        return OS_ERR_OBJ_DEPLETED;
    }
    OSExitCriticalSection(cpu_sr);

    //! Set object type.
    //! init mutex property.
    //! init mutex wait list head.
    pmutex->OSObjType           = OS_OBJ_TYPE_SET(OS_OBJ_TYPE_MUTEX)
                                | OS_OBJ_TYPE_WAITABLE_MSK
                                | OS_OBJ_PRIO_TYPE_SET(OS_OBJ_PRIO_TYPE_PRIO_LIST);
    pmutex->OSMutexCeilingPrio  = ceilingPrio;
    pmutex->OSMutexOwnerPrio    = OS_TASK_IDLE_PRIO;
    pmutex->OSMutexOwnerTCB     = NULL;
    os_list_init_head(&pmutex->OSMutexWaitList);
    
    //! reg waitable object.
    OSEnterCriticalSection(cpu_sr);
    OS_RegWaitableObj((OS_WAITABLE_OBJ *)pmutex);
    OSExitCriticalSection(cpu_sr);

    *pMutexHandle = pmutex;
    
    return OS_ERR_NONE;
}

/*!
 *! \Brief       DELETE A MUTEX
 *!
 *! \Description This function deletes a mutex and readies all tasks pending 
 *!              on the it.
 *!
 *! \Arguments   hMutex         is a handle to the event control block associated with the
 *!                             desired mutex.
 *!
 *!              opt            determines delete options as follows:
 *!                             opt == OS_DEL_NO_PEND   Delete mutex ONLY if no task pending
 *!                             opt == OS_DEL_ALWAYS    Deletes the mutex if tasks are waiting.
 *!                                                     In this case, all the tasks pending will
 *!                                                     be readied.
 *!
 *! \Returns     OS_ERR_NONE            The call was successful and the mutex was deleted
 *!              OS_ERR_USE_IN_ISR      If you attempted to delete the MUTEX from an ISR
 *!              OS_ERR_INVALID_OPT     An invalid option was specified
 *!              OS_ERR_TASK_WAITING    One or more tasks were waiting on the mutex
 *!              OS_ERR_INVALID_HANDLE  If 'hMutex' is an invalid handle.
 *!              OS_ERR_OBJ_TYPE      If you didn't pass a event mutex object.
 *!
 *! \Notes       1) This function must be used with care.  Tasks that would normally expect the
 *!                 presence of the mutex MUST check the return code of osMutexPend().
 *!              2) This call can potentially disable interrupts for a long time.  The interrupt
 *!                 disable time is directly proportional to the number of tasks waiting on the mutex.
 *!              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful
 *!                 because the resource(s) will no longer be guarded by the mutex.
 *!              4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that the owner of the Mutex
 *!                            (if there is one) is ready-to-run and is thus NOT pending on another
 *!                            kernel object or has delayed itself.  In other words, if a task owns
 *!                            the mutex being deleted, that task will be made ready-to-run at its
 *!                            original priority.
 */
#if OS_MUTEX_DEL_EN > 0u
OS_ERR osMutexDelete(OS_HANDLE *pMutexHandle, UINT8 opt)
{
    OS_MUTEX   *pmutex;
    OS_TCB     *powner;
    BOOL        taskPend;
    BOOL        taskSched = FALSE;
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (pMutexHandle == NULL) {                 //!< Validate pMutexHandle
        return OS_ERR_INVALID_HANDLE;
    }
#endif
    if (osIntNesting > 0u) {                    //!< Can't DELETE from an ISR.
        return OS_ERR_USE_IN_ISR;
    }
    if (*pMutexHandle == NULL) {                //!< Validate handle
        return OS_ERR_INVALID_HANDLE;
    }
    pmutex = (OS_MUTEX *)*pMutexHandle;
    if (OS_OBJ_TYPE_GET(pmutex->OSObjType) != OS_OBJ_TYPE_MUTEX) {   //!< Validate event block type
        return OS_ERR_OBJ_TYPE;
    }

    OSEnterCriticalSection(cpu_sr);
    if (pmutex->OSMutexWaitList.Next != &pmutex->OSMutexWaitList) {     //!< See if any tasks waiting on mutex
        taskPend    = TRUE;                                             //!< Yes
        taskSched   = TRUE;
    } else {
        taskPend    = FALSE;                                            //!< No
    }
    switch (opt) {
        case OS_DEL_NO_PEND:                                            //!< Delete mutex if NO task waiting.
            if (taskPend != FALSE) {
                OSExitCriticalSection(cpu_sr);
                return OS_ERR_TASK_WAITING;
            }
            break;

        case OS_DEL_ALWAYS:
            break;

        default:
             OSExitCriticalSection(cpu_sr);
             return OS_ERR_INVALID_OPT;
    }
    OS_DeregWaitableObj((OS_WAITABLE_OBJ *)pmutex);
    
    powner = pmutex->OSMutexOwnerTCB;
    if (powner != NULL) {                                               //!< See if this mutex has been owned by any task.
        powner->OSTCBOwnMutex = NULL;                                   //!< Yes.
        if (pmutex->OSMutexOwnerPrio != powner->OSTCBPrio) {            //!< If this task's prio has been changed,
            OS_ChangeTaskPrio(powner, pmutex->OSMutexOwnerPrio);        //!< Yes, restore task's prio.
            taskSched = TRUE;
        }
    }
    
    while (pmutex->OSMutexWaitList.Next != &pmutex->OSMutexWaitList) {  //!< Ready ALL tasks are pending for this mutex.
        OS_WaitableObjRdyTask((OS_WAITABLE_OBJ *)pmutex, OS_STAT_PEND_ABORT);
    }
    pmutex->OSObjType           = OS_OBJ_TYPE_UNUSED;
    pmutex->OSMutexCeilingPrio  = 0u;
    pmutex->OSMutexOwnerTCB     = NULL;
    pmutex->OSMutexOwnerPrio    = 0u;
    OS_ObjPoolFree(&osMutexFreeList, pmutex);
    OSExitCriticalSection(cpu_sr);
    
    if (taskSched) {
        OS_ScheduleRunPrio();
    }
    
    return OS_ERR_NONE;
}
#endif

/*!
 *! \Brief       PEND ON MUTEX
 *!
 *! \Description This function waits for a mutex.
 *!
 *! \Arguments   hMutex        is a handle to the mutex.
 *!
 *!              timeout       is an optional timeout period (in clock ticks).  If non-zero, your
 *!                            task will wait for the resource up to the amount of time specified
 *!                            by this argument. If you specify OS_INFINITE, however, your task
 *!                            will wait forever at the specified mutex or, until the resource
 *!                            becomes available. If you specify 0, however, your task will NOT wait
 *!                            and return OS_ERR_TIMEOUT if the specified mutex is not avalible.
 *!
 *! \Returns     OS_ERR_NONE            The call was successful and your task owns the mutex
 *!              OS_ERR_INVALID_HANDLE  If 'hMutex' is an invalid handle.
 *!              OS_ERR_OBJ_TYPE      If object was not a mutex.
 *!              OS_ERR_USE_IN_ISR      If this function was called from an ISR and the result
 *!              OS_ERR_PEND_LOCKED     If this function was called when the scheduler is locked
 *!              OS_ERR_TIMEOUT         The mutex was not available within the specified 'timeout'.
 *!              OS_ERR_PEND_ABORT      The wait on the mutex was aborted.
 *!                                     would lead to a suspension.
 *!              OS_ERR_OVERLAP_MUTEX   Current task overlap the same mutex. User should release the
 *!                                     mutex before try getting it again.
 *!
 *! \Notes       1) The task that owns the mutex MUST NOT try to owns the same mutex. It's say it
 *!                 could not be recursive.
 */
OS_ERR osMutexPend(OS_HANDLE hMutex, UINT32 timeout)
{
    OS_MUTEX       *pmutex = (OS_MUTEX *)hMutex;
    OS_WAIT_NODE    node;
#if OS_CRITICAL_METHOD == 3u                //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif
    OS_ERR          err;
    UINT8           prio;


#if OS_ARG_CHK_EN > 0u
    if (hMutex == NULL) {                   //!< Validate hMutex
        return OS_ERR_INVALID_HANDLE;
    }
#endif
    if (osIntNesting > 0u) {                //!< See if called from ISR ...
        return OS_ERR_USE_IN_ISR;           //!< ... can't PEND from an ISR
    }
    if (osLockNesting > 0u) {               //!< See if called with scheduler locked ...
        return OS_ERR_PEND_LOCKED;          //!< ... can't PEND when locked
    }
    if (OS_OBJ_TYPE_GET(pmutex->OSObjType) != OS_OBJ_TYPE_MUTEX) {  //!< Validate event block type
        return OS_ERR_OBJ_TYPE;
    }

    OSEnterCriticalSection(cpu_sr);
    if (pmutex->OSMutexOwnerTCB == NULL) {              //!< Is Mutex available?
        pmutex->OSMutexOwnerPrio = osTCBCur->OSTCBPrio; //!< Yes, current task own this mutex.
        pmutex->OSMutexOwnerTCB  = osTCBCur;            //!< ... save current task's TCB and prio to mutex.
        osTCBCur->OSTCBOwnMutex  = pmutex;
        OSExitCriticalSection(cpu_sr);
        return OS_ERR_NONE;
    }
    
    if (pmutex->OSMutexOwnerTCB == osTCBCur) {
        OSExitCriticalSection(cpu_sr);
        return OS_ERR_OVERLAP_MUTEX;
    }
    
    if (timeout == 0u) {
        OSExitCriticalSection(cpu_sr);
        return OS_ERR_TIMEOUT;
    }

    if (pmutex->OSMutexCeilingPrio < osTCBCur->OSTCBPrio) {
        prio = pmutex->OSMutexCeilingPrio;
    } else {
        prio = osTCBCur->OSTCBPrio;
    }
    if (pmutex->OSMutexOwnerTCB->OSTCBPrio > prio) {            //!< Is owner has a lower priority?
        OS_ChangeTaskPrio(pmutex->OSMutexOwnerTCB, prio);   //!< Yes. Rise owner's priority.
    }

    OS_WaitableObjAddTask((OS_WAITABLE_OBJ *)pmutex, &node, timeout);           //!< Suspend current task.
    OSExitCriticalSection(cpu_sr);
    OS_ScheduleRunNext();

    switch (node.OSWaitNodeRes) {
        case OS_STAT_PEND_OK:
             err = OS_ERR_NONE;
             break;

        case OS_STAT_PEND_ABORT:
             err = OS_ERR_PEND_ABORT;
             break;

        case OS_STAT_PEND_TO:
        default:
             err = OS_ERR_TIMEOUT;
             break;
    }
    
    return err;
}

/*!
 *! \Brief       POST TO A MUTEX
 *!
 *! \Description This function signals a mutex
 *!
 *! \Arguments   hMutex         is a handle to the mutex.
 *!
 *! \Returns     OS_ERR_NONE                The call was successful and the mutex was signaled.
 *!              OS_ERR_USE_IN_ISR            Attempted to post from an ISR (not valid for MUTEXes)
 *!              OS_ERR_INVALID_HANDLE      If 'hMutex' is an invalid handle.
 *!              OS_ERR_OBJ_TYPE          If you didn't pass a event mutex object.
 *!              OS_ERR_NOT_MUTEX_OWNER     The task that did the post is NOT the owner of the MUTEX.
 *!
 *! \Notes       1) The mutex can ONLY be released by it's owner.
 */
OS_ERR osMutexPost(OS_HANDLE hMutex)
{
    OS_MUTEX   *pmutex = (OS_MUTEX *)hMutex;
    OS_TCB     *ptcb;
    BOOL        taskSched = FALSE;
#if OS_CRITICAL_METHOD == 3u                //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (hMutex == NULL) {                   //!< Validate hMutex
        return OS_ERR_INVALID_HANDLE;
    }
#endif
    if (osIntNesting > 0u) {                //!< See if called from ISR ...
        return OS_ERR_USE_IN_ISR;           //!< ... can't POST mutex from an ISR
    }
    if (OS_OBJ_TYPE_GET(pmutex->OSObjType) != OS_OBJ_TYPE_MUTEX) {   //!< Validate event block type
        return OS_ERR_OBJ_TYPE;
    }

    OSEnterCriticalSection(cpu_sr);
    if (osTCBCur != pmutex->OSMutexOwnerTCB) {      //!< See if this task owns the mutex.
        OSExitCriticalSection(cpu_sr);
        return OS_ERR_NOT_MUTEX_OWNER;
    }
    
    osTCBCur->OSTCBOwnMutex = NULL;
    if (pmutex->OSMutexOwnerPrio != osTCBCur->OSTCBPrio) {          //!< If this task's prio has been changed...
        OS_ChangeTaskPrio(osTCBCur, pmutex->OSMutexOwnerPrio);  //!< ... Yes, restore task's prio.
        taskSched = TRUE;
    }
    
    if (pmutex->OSMutexWaitList.Next != &pmutex->OSMutexWaitList) {         //!< Any task waiting for the mutex?
        ptcb = OS_WaitableObjRdyTask((OS_WAITABLE_OBJ *)pmutex, OS_STAT_PEND_OK); //!< Yes, Make HPT waiting for mutex ready
        pmutex->OSMutexOwnerTCB  = ptcb;
        pmutex->OSMutexOwnerPrio = ptcb->OSTCBPrio;
        taskSched = TRUE;
    } else {                                                                //!< No.
        pmutex->OSMutexOwnerTCB  = NULL;
        pmutex->OSMutexOwnerPrio = OS_TASK_IDLE_PRIO;
    }
    OSExitCriticalSection(cpu_sr);
    
    if (taskSched) {
        OS_ScheduleRunPrio();
    }
    
    return OS_ERR_NONE;
}

/*!
 *! \Brief       QUERY A MUTEX
 *!
 *! \Description This function obtains information about a mutex
 *!
 *! \Arguments   hMutex         is a handle to the mutex.
 *!
 *!              pInfo          is a pointer to a structure that will contain information about
 *!                             the mutex
 *!
 *! \Returns     OS_ERR_NONE            The call was successful and the message was sent
 *!              OS_ERR_INVALID_HANDLE  If 'hMutex' is an invalid handle.
 *!              OS_ERR_OBJ_TYPE      If you didn't pass a event mutex object.
 *!              OS_ERR_PDATA_NULL      If 'pInfo' is a NULL pointer
 */
#if OS_MUTEX_QUERY_EN > 0u
OS_ERR osMutexQuery(OS_HANDLE hMutex, OS_MUTEX_INFO *pInfo)
{
    OS_MUTEX       *pmutex = (OS_MUTEX *)hMutex;
#if OS_CRITICAL_METHOD == 3u            //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


#if OS_ARG_CHK_EN > 0u
    if (hMutex == NULL) {               //!< Validate hMutex
        return OS_ERR_INVALID_HANDLE;
    }
    if (pInfo == NULL) {                //!< Validate pInfo
        return OS_ERR_PDATA_NULL;
    }
#endif
    if (OS_OBJ_TYPE_GET(pmutex->OSObjType) != OS_OBJ_TYPE_MUTEX) {  //!< Validate event block type
        return OS_ERR_OBJ_TYPE;
    }

    OSEnterCriticalSection(cpu_sr);
    if (pmutex->OSMutexOwnerTCB == NULL) {                //!< Any task own this mutex?
        pInfo->OSValue       = TRUE;
        pInfo->OSOwnerPrio   = 0u;
        pInfo->OSOwnerTCB    = NULL;
    } else {
        pInfo->OSValue       = FALSE;
        pInfo->OSOwnerTCB    = pmutex->OSMutexOwnerTCB;
        pInfo->OSOwnerPrio   = pmutex->OSMutexOwnerTCB->OSTCBPrio;
    }
    pInfo->OSCeilingPrio     = pmutex->OSMutexCeilingPrio;
    pInfo->OSWaitList        = pmutex->OSMutexWaitList;     //!< Copy wait list
    OSExitCriticalSection(cpu_sr);
    
    return OS_ERR_NONE;
}
#endif      //!< #if OS_MUTEX_QUERY_EN > 0u

#endif      //!< #if (OS_MUTEX_EN > 0u) && (OS_MAX_MUTEXES >0u)
