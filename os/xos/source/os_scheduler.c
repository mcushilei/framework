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
#define __OS_SCHEDULER_C__

/*============================ INCLUDES ======================================*/
#include ".\os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define OS_COUNT_LEADING_ZERO(__B)      (OSUnMapTbl[__B])

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*!
 *! \Brief       PRIORITY RESOLUTION TABLE
 *!
 *! \Notes       Index into table is bit pattern to resolve highest priority
 *!              Indexed value corresponds to highest priority bit position (i.e. 0..7)
 *!              Leading 0 algorithm.
 */
static const UINT8 OSUnMapTbl[256] = {
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x00 to 0x0F
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x10 to 0x1F
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x20 to 0x2F
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x30 to 0x3F
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x40 to 0x4F
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x50 to 0x5F
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x60 to 0x6F
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x70 to 0x7F
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x80 to 0x8F
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0x90 to 0x9F
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0xA0 to 0xAF
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0xB0 to 0xBF
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0xC0 to 0xCF
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0xD0 to 0xDF
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, //!< 0xE0 to 0xEF
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  //!< 0xF0 to 0xFF
};

/*============================ IMPLEMENTATION ================================*/

/*!
 *! \Brief       INITIALIZE THE READY LIST
 *!
 *! \Description This function is called by osInit() to initialize the Ready List.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
void OS_ScheduleInit(void)
{
    UINT16 i;


    osRdyGrp = 0u;
    for (i = 0u; i < OS_BITMAP_TBL_SIZE; i++) {
        osRdyTbl[i] = 0u;
    }

    for (i = 0u; i < OS_MAX_PRIO_LEVELS; i++) {
        os_list_init_head(&osRdyList[i]);
    }

    osTCBCur      = NULL;
    osTCBNextRdy  = NULL;
}

void OS_LockSched(void)
{
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (osRunning != FALSE) {                   //!< Make sure multitasking is running
        if (osIntNesting == 0u) {               //!< Can't call from an ISR
            OSEnterCriticalSection(cpu_sr);
            if (osLockNesting < 255u) {         //!< Prevent osLockNesting from wrapping back to 0
                osLockNesting++;                //!< Increment lock nesting level
            }
            OSExitCriticalSection(cpu_sr);
        }
    }
}

void OS_UnlockSched(void)
{
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (osRunning != FALSE) {                   //!< Make sure multitasking is running
        if (osIntNesting == 0u) {               //!< Can't call from an ISR
            OSEnterCriticalSection(cpu_sr);
            if (osLockNesting > 0u) {           //!< Do not decrement if already 0
                osLockNesting--;                //!< Decrement lock nesting level
            }
            OSExitCriticalSection(cpu_sr);
        }
    }
}

void OS_ScheduleReadyTask(OS_TCB *ptcb)
{
    UINT8 prio, g;
    OS_PRIO x, y;
    
    os_list_add(&ptcb->OSTCBList, osRdyList[ptcb->OSTCBPrio].Prev); //!< add task to the end of ready task list.
    
    prio = ptcb->OSTCBPrio;
    
#if OS_MAX_PRIO_LEVELS <= 64u                        //!< See if we support up to 64 tasks
    g = (prio >> 3) & 0x07u;
    x = 1u << (prio & 0x07u);
#else
    g = (prio >> 4) & 0x0Fu;
    x = 1u << (prio & 0x0Fu);
#endif
    y = 1u << g;
    
    osRdyGrp    |= y;                  //!< Make this priority has task ready-to-run.
    osRdyTbl[g] |= x;
}

void OS_ScheduleUnreadyTask(OS_TCB *ptcb)
{
    UINT8 prio, g;
    OS_PRIO x, y;
    
    os_list_del(&ptcb->OSTCBList);

    prio = ptcb->OSTCBPrio;
    
    if (osRdyList[prio].Prev == &osRdyList[prio]) {
#if OS_MAX_PRIO_LEVELS <= 64u                        //!< See if we support up to 64 tasks
        g = (prio >> 3) & 0x07u;
        x = 1u << (prio & 0x07u);
#else
        g = (prio >> 4) & 0x0Fu;
        x = 1u << (prio & 0x0Fu);
#endif
        y = 1u << g;

        osRdyTbl[g] &= (OS_PRIO)~x;
        if (osRdyTbl[g] == 0u) {
            osRdyGrp &= (OS_PRIO)~y;
        }
    }
}

static UINT8 os_schedule_get_highest_prio(void)
{
    UINT8   y;
    UINT8   prio;
    OS_PRIO tblVal;


    //! find the highest priority of ready task.
#if OS_MAX_PRIO_LEVELS <= 64u               //!< See if we support up to 64 tasks
    y       = OS_COUNT_LEADING_ZERO(osRdyGrp);
    tblVal  = osRdyTbl[y];
    prio    = (y * 8u) + OS_COUNT_LEADING_ZERO(tblVal);
#else                                       //!< We support up to 256 tasks
    if ((osRdyGrp & 0xFFu) != 0u) {
        y =      OS_COUNT_LEADING_ZERO(osRdyGrp & 0xFFu);
    } else {
        y = 8u + OS_COUNT_LEADING_ZERO((osRdyGrp >> 8u) & 0xFFu);
    }
    tblVal = osRdyTbl[y];
    if ((tblVal & 0xFFu) != 0u) {
        prio = (y * 16u) +      OS_COUNT_LEADING_ZERO(tblVal & 0xFFu);
    } else {
        prio = (y * 16u) + 8u + OS_COUNT_LEADING_ZERO((tblVal >> 8u) & 0xFFu);
    }
#endif
    
    return prio;
}

/*!
 *! \Brief       CHANGE PRIORITY OF A TASK
 *!
 *! \Description This function changes the priority of a task.
 *!
 *! \Arguments   ptcb     pointer to tcb
 *!
 *!              newp     is the new priority
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function assumes that interrupts are disabled.
 *!              2) This function is INTERNAL to OS and your application should not call it.
 */
void OS_ScheduleChangePrio(OS_TCB *ptcb, UINT8 newprio)
{
    OS_WAITABLE_OBJ    *pobj;
    OS_WAIT_NODE       *pnode;
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    OSEnterCriticalSection(cpu_sr);
    pnode = ptcb->OSTCBWaitNode;
    if (pnode != NULL) {
        pobj = pnode->OSWaitNodeECB;
        if (pobj != NULL) {                     //!< Is this task pending for any event?...
            if (OS_OBJ_PRIO_TYPE_GET(pobj->OSObjType) == OS_OBJ_PRIO_TYPE_PRIO_LIST) {  //!< ...Yes. Has this event a prio-wait list?
                OS_LIST_NODE *list;
                OS_WAIT_NODE *nextNode;
                
                os_list_del(&pnode->OSWaitNodeList);                //!< remove wait node from old priority.
                for (list = pobj->OSWaitObjWaitNodeList.Next; list != &pobj->OSWaitObjWaitNodeList; list = list->Next) {    //!< find new position
                    nextNode = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
                    if (newprio < nextNode->OSWaitNodeTCB->OSTCBPrio) {
                        break;
                    }
                }
                os_list_add(&pnode->OSWaitNodeList, list->Prev);    //!< and place at the new place.
            }
        }
        ptcb->OSTCBPrio = newprio;              //!< Set new task priority
    } else {
        OS_ScheduleUnreadyTask(ptcb);           //!< Remove TCB from old priority
        ptcb->OSTCBPrio = newprio;              //!< Set new task priority        
        OS_ScheduleReadyTask(ptcb);             //!< Place TCB @ new priority
    }
    OSExitCriticalSection(cpu_sr);
}

/*!
 *! \Brief       FIND HIGHEST PRIORITY TASK READY TO RUN
 *!
 *! \Description This function is called by other OS services to determine the highest priority task
 *!              that is ready to run.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function is INTERNAL to OS and your application should not call it.
 *!              2) Interrupts are assumed to be DISABLED when this function is called.
 */

void OS_SchedulePrio(void)
{
    UINT8           prio;
    OS_LIST_NODE   *node;


    prio = os_schedule_get_highest_prio();
    if (prio != osTCBCur->OSTCBPrio) {
        node = osRdyList[prio].Next;
        os_list_del(node);
        os_list_add(node, osRdyList[prio].Prev);
        osTCBNextRdy = OS_CONTAINER_OF(node, OS_TCB, OSTCBList);
    }
}

/*!
 *! \Brief       SCHEDULER
 *!
 *! \Description This function is called by other OS services to run the next task, beacuse
 *!              current task has been pend.  This function is invoked by TASK level code
 *!              and is NOT used to reschedule tasks from ISRs (see osIntExit() for ISR rescheduling).
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function is INTERNAL to OS and your application should not call it.
 *!              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
 */

void OS_ScheduleNext(void)
{
    UINT8           prio;
    OS_LIST_NODE   *node;


    prio = os_schedule_get_highest_prio();
    node = osRdyList[prio].Next;
    os_list_del(node);
    os_list_add(node, osRdyList[prio].Prev);
    osTCBNextRdy = OS_CONTAINER_OF(node, OS_TCB, OSTCBList);
}

void OS_ScheduleRunPrio(void)
{
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif

    if (osIntNesting != 0u) {                   //!< Can not be used in ISR and ...
        return;
    }
    
    if (osLockNesting != 0u) {                  //!< ... scheduler is not locked
        return;
    }

    OSEnterCriticalSection(cpu_sr);
    OS_SchedulePrio();
    if (osTCBNextRdy != osTCBCur) {
        OSExitCriticalSection(cpu_sr);
        OSCtxSw();                              //!< Perform a context switch
        return;
    }
    OSExitCriticalSection(cpu_sr);
}

void OS_ScheduleRunNext(void)
{
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif

    if (osIntNesting != 0u) {                   //!< Can not be used in ISR and ...
        return;
    }
    
    if (osLockNesting != 0u) {                  //!< ... scheduler is not locked
        return;
    }

    OSEnterCriticalSection(cpu_sr);
    OS_ScheduleNext();
    if (osTCBNextRdy != osTCBCur) {
        OSExitCriticalSection(cpu_sr);
        OSCtxSw();                              //!< Perform a context switch
        return;
    }
    OSExitCriticalSection(cpu_sr);
}


/* EOF */
