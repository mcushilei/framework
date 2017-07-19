
//! \note do not move this pre-processor statement to other places
#define  __OS_CORE_C__

/*============================ INCLUDES ======================================*/
#include ".\os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define OS_COUNT_LEADING_ZERO(__B)      (OSUnMapTbl[__B])

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
static  void    os_init_obj_list(void);

static  void    os_init_misc(void);

static  void    os_schedule_init(void);

#if OS_STAT_EN > 0u
static  void    os_init_statistics_task(void);
static  void    os_task_statistics(void *parg);
#endif

static  void    os_init_idle_task(void);
static  void    os_task_idle(void *parg);

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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

/*!
 *! \Brief       INITIALIZATION
 *!
 *! \Description This function is used to initialize the internals of OS and MUST be called prior to
 *!              creating any OS object and, prior to calling osStart().
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
void osInit(void)
{
#if OS_CPU_HOOKS_EN > 0
    OSInitHookBegin();                                          //!< Call port specific initialization code
#endif

    os_init_obj_list();                                           //!< Initialize the free list of OS_TCBs

    os_init_misc();                                              //!< Initialize miscellaneous variables

    os_schedule_init();                                          //!< Initialize the Ready List

    os_init_idle_task();                                          //!< Create the Idle Task
#if OS_STAT_EN > 0u
    os_init_statistics_task();                                          //!< Create the Statistic Task
#endif

#if OS_CPU_HOOKS_EN > 0u
    OSInitHookEnd();                                            //!< Call port specific init. code
#endif

#if OS_DEBUG_EN > 0u
    OSDebugInit();
#endif
}

/*!
 *! \Brief       INITIALIZE MISCELLANEOUS VARIABLES
 *!
 *! \Description This function is called by osInit() to initialize miscellaneous variables.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
static void os_init_misc(void)
{
    osIntNesting             = 0u;                        //!< Clear the interrupt nesting counter
    osLockNesting            = 0u;                        //!< Clear the scheduling lock counter

    osTaskCtr                = 0u;                        //!< Clear the number of tasks

    osRunning                = FALSE;                  //!< Indicate that multitasking not started

    osIdleCtr                = 0u;                        //!< Clear the idle counter

#if OS_STAT_EN > 0u
    osCtxSwCtr               = 0u;                        //!< Clear the context switch counter
    osCPUUsage               = 0u;
    osIdleCtrMax             = 0u;
    osStatRunning            = FALSE;                  //!< Statistic task is not ready
#endif
}

/*!
 *! \Brief       INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
 *!
 *! \Description This function is called by osInit() to initialize the free list of task control blocks.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
static bool os_obj_pool_init(OS_LIST_NODE **ppObj, void *pMem, UINT16 num, UINT16 objSize)
{
    OS_LIST_NODE *pobj;
    
    for (; num; num--) {
        pobj = (OS_LIST_NODE *)pMem;
        *ppObj = pobj;
        pobj->Next = NULL;
        pobj->Prev = NULL;
        ppObj = &pobj->Next;
        pMem = (UINT8 *)pMem + objSize;
    }

    return true;
}

bool OS_ObjPoolFree(OS_LIST_NODE **ppObj, void *pobj)
{    
    ((OS_LIST_NODE *)pobj)->Next = *ppObj;
    ((OS_LIST_NODE *)pobj)->Prev = NULL;
    *ppObj = (OS_LIST_NODE *)pobj;

    return true;
}

void *OS_ObjPoolNew(OS_LIST_NODE **ppObj)
{
    OS_LIST_NODE *pobj;

    if (*ppObj == NULL) {
        return NULL;
    }

    pobj   = *ppObj;
    *ppObj = pobj->Next; 
    pobj->Next = NULL;

    return pobj;
}


static void os_init_obj_list(void)
{
    OS_MemClr((UINT8 *)osTCBFreeTbl, sizeof(osTCBFreeTbl));
    os_obj_pool_init(&osTCBFreeList, osTCBFreeTbl, sizeof(osTCBFreeTbl) / sizeof(OS_TCB), sizeof(OS_TCB));

#if (OS_FLAG_EN | OS_MUTEX_EN | OS_SEMP_EN)
    OS_MemClr((UINT8 *)osSempFreeTbl, sizeof(osSempFreeTbl));
    os_obj_pool_init(&osSempFreeList, osSempFreeTbl, sizeof(osSempFreeTbl) / sizeof(OS_SEMP), sizeof(OS_SEMP));

    OS_MemClr((UINT8 *)osMutexFreeTbl, sizeof(osMutexFreeTbl));
    os_obj_pool_init(&osMutexFreeList, osMutexFreeTbl, sizeof(osMutexFreeTbl) / sizeof(OS_MUTEX), sizeof(OS_MUTEX));

    OS_MemClr((UINT8 *)osFlagFreeTbl, sizeof(osFlagFreeTbl));
    os_obj_pool_init(&osFlagFreeList, osFlagFreeTbl, sizeof(osFlagFreeTbl) / sizeof(OS_FLAG), sizeof(OS_FLAG));
#endif
}

/*!
 *! \Brief       INITIALIZE THE READY LIST
 *!
 *! \Description This function is called by osInit() to initialize the Ready List.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
static void os_schedule_init(void)
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
    
    os_list_init_head(&osPndList);
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

void OS_SchedulePendTask(OS_TCB *ptcb, UINT32 ticks)
{
    if (ticks == OS_INFINITE) {
        ticks = 0u;
    }
    ptcb->OSTCBDly = ticks;
    
    os_list_add(&ptcb->OSTCBList, osPndList.Prev);  //!< add task to the end of PEND task list.
}

void OS_ScheduleUnpendTask(OS_TCB *ptcb)
{
    os_list_del(&ptcb->OSTCBList);
    ptcb->OSTCBDly = 0u;
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
#if (OS_EVENT_EN)
    OS_WAITBALE_OBJ    *pobj;
    OS_WAIT_NODE       *pnode;
#endif
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    OSEnterCriticalSection(cpu_sr);
    if (ptcb->OSTCBDly != 0u) {
#if (OS_EVENT_EN)
        pnode = ptcb->OSTCBWaitNode;
        if (pnode != NULL) {                                    //!< Is this task pending for any event?
            pobj = pnode->OSWaitNodeECB;
            if (OS_OBJ_PRIO_TYPE_GET(pobj->OSObjType) == OS_OBJ_PRIO_TYPE_PRIO_LIST) {
                OS_LIST_NODE* list;
                OS_WAIT_NODE* nextNode;
                
                os_list_del(&pnode->OSWaitNodeList);
                
                for (list = pobj->OSWaitObjWaitList.Next; list != &pobj->OSWaitObjWaitList; list = list->Next) {
                    nextNode = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
                    if (newprio < nextNode->OSWaitNodeTCB->OSTCBPrio) {
                        break;
                    }
                }
                os_list_add(&pnode->OSWaitNodeList, list->Prev);
            }
        }
#endif
        ptcb->OSTCBPrio = newprio;                                  //!< Set new task priority
    } else {
        OS_ScheduleUnreadyTask(ptcb);                        //!< Remove TCB from old priority
        ptcb->OSTCBPrio = newprio;
        OS_ScheduleReadyTask(ptcb);                           //!< Place TCB @ new priority
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
    UINT8   prio;
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
 *! \Description This function is called by other OS services to determine whether a new, high
 *!              priority task has been made ready to run.  This function is invoked by TASK level code
 *!              and is not used to reschedule tasks from ISRs (see osIntExit() for ISR rescheduling).
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function is INTERNAL to OS and your application should not call it.
 *!              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
 */
void OS_Schedule(void)
{
    UINT8   prio;
    OS_LIST_NODE   *node;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    OSEnterCriticalSection(cpu_sr);
    if (osIntNesting == 0u) {                           //!< Schedule only if all ISRs done and ...
        if (osLockNesting == 0u) {                      //!< ... scheduler is not locked
            OS_SchedulePrio();
            if (osTCBNextRdy != osTCBCur) {             //!< No Ctx Sw if current task is highest rdy
                OSExitCriticalSection(cpu_sr);
                OSCtxSw();                              //!< Perform a context switch
            }
        }
    }
    OSExitCriticalSection(cpu_sr);
}

/*!
 *! \Brief       ENTER ISR
 *!
 *! \Description This function is used to notify OS that you are about to service an interrupt
 *!              service routine (ISR).  This allows OS to keep track of interrupt nesting and thus
 *!              only perform rescheduling at the last nested ISR.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) You MUST invoke osIntEnter() and osIntExit() in pair.  In other words, for every call
 *!                 to osIntEnter() at the beginning of the ISR you MUST have a call to osIntExit() at the
 *!                 end of the ISR.
 *!              2) You are allowed to nest interrupts up to 255 levels deep.
 */
void osIntEnter(void)
{
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    if (osRunning == FALSE) {
        return;
    }
    
    OSEnterCriticalSection(cpu_sr);
    if (osIntNesting < 255u) {
        osIntNesting++;                 //!< Increment ISR nesting level
    }
    OSExitCriticalSection(cpu_sr);
}

/*!
 *! \Brief       EXIT ISR
 *!
 *! \Description This function is used to notify os that you have completed servicing an ISR.  When
 *!              the last nested ISR has completed, OS will call the scheduler to determine whether
 *!              a new, high-priority task, is ready to run.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) You MUST invoke osIntEnter() and osIntExit() in pair.  In other words, for every call
 *!                 to osIntEnter() at the beginning of the ISR you MUST have a call to osIntExit() at the
 *!                 end of the ISR.
 *!              2) Rescheduling is prevented when the scheduler is locked.
 */
void osIntExit(void)
{
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    if (osRunning == FALSE) {
        return;
    }
    
    OSEnterCriticalSection(cpu_sr);
    if (osIntNesting > 0u) {                            //!< Prevent osIntNesting from wrapping
        osIntNesting--;
    }
    if (osIntNesting == 0u) {                           //!< Reschedule only if all ISRs complete ...
        if (osLockNesting == 0u) {                      //!< ... and scheduler is not locked.
            OS_SchedulePrio();
            if (osTCBNextRdy != osTCBCur) {             //!< No Ctx Sw if current task is highest rdy
                OSExitCriticalSection(cpu_sr);
                OSIntCtxSw();                           //!< Perform interrupt level ctx switch
                return;
            }
        }
    }
    OSExitCriticalSection(cpu_sr);
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

/*!
 *! \Brief       PREVENT SCHEDULING
 *!
 *! \Description This function is used to prevent rescheduling to take place.  This allows your application
 *!              to prevent context switches until you are ready to permit context switching.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) You MUST invoke osLockSched() and osUnlockSched() in pair.  In other words, for every
 *!                 call to osLockSched() you MUST have a call to osUnlockSched().
 */
void osLockSched(void)
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

/*!
 *! \Brief       ENABLE SCHEDULING
 *!
 *! \Description This function is used to re-allow rescheduling.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Notes       1) You MUST invoke osLockSched() and osUnlockSched() in pair.  In other words, for every
 *!                 call to osLockSched() you MUST have a call to osUnlockSched().
 */
void osUnlockSched(void)
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
            OS_Schedule();
        }
    }
}

/*!
 *! \Brief       START MULTITASKING
 *!
 *! \Description This function is used to start the multitasking process which lets OS manages the
 *!              task that you have created.  Before you can call osStart(), you MUST have called osInit()
 *!              and you MUST have created at least one task.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 *!
 *! \Note        OSStartHighRdy() MUST:
 *!                 a) Call OSTaskSwHook() then,
 *!                 b) Set osRunning to TRUE.
 *!                 c) Load the context of the task pointed to by osTCBNextRdy.
 *!                 d) Execute the task.
 *!                 e) Enable system-level interrupt.
 */
void osStart(void)
{
    if (osRunning == FALSE) {
        OS_SchedulePrio();
        osTCBCur = osTCBNextRdy;
        OSStartHighRdy();                       //!< Execute target specific code to start task
    }
}

/*!
 *! \Brief       PROCESS SYSTEM TICK
 *!
 *! \Description This function is used to signal to OS the occurrence of a 'system tick' (also known
 *!              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
 *!              called by a HIGH priority task.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
void osTimeTick(void)
{
    OS_LIST_NODE   *node;
    OS_TCB         *ptcb;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
    OSTimeTickHook();
#endif
    
    if (osRunning != FALSE) {
        for (node = osPndList.Next; node != &osPndList; ) {      //!< Go through all task in TCB list.
            ptcb  = OS_CONTAINER_OF(node, OS_TCB, OSTCBList);
            node = node->Next;
            
            if ((ptcb->OSTCBDly != 0u) &&
                (ptcb->OSTCBDly != OS_INFINITE)) {
                ptcb->OSTCBDly--;
                if (ptcb->OSTCBDly == 0u) {                                     //!< If timeout
                    if (ptcb->OSTCBWaitNode != NULL) {                          //!< See if waiting for any event
                        ptcb->OSTCBWaitNode->OSWaitNodeRes = OS_STAT_PEND_TO;   //!< ...Indicate PEND timeout.
                        OS_EventTaskRemove(ptcb);                               //!< Yes, Remove from any event and...
                    } else {
                        OS_ScheduleUnpendTask(ptcb);
                    }
                    OSEnterCriticalSection(cpu_sr);
                    OS_ScheduleReadyTask(ptcb);
                    OSExitCriticalSection(cpu_sr);
                }
            }
        }
    }
}

/*!
 *! \Brief       DELAY TASK 'n' TICKS
 *!
 *! \Description This function is called to delay execution of the currently running task until the
 *!              specified number of system ticks expires.  This, of course, directly equates to delaying
 *!              the current task for some time to expire.  No delay will result If the specified delay is
 *!              0.  If the specified delay is greater than 0 then, a context switch will result.
 *!
 *! \Arguments   ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
 *!                        Note that by specifying 0, the task will not be delayed.
 *!
 *! \Returns     none
 */
void osTimeDelay(UINT32 ticks)
{
#if OS_CRITICAL_METHOD == 3u                        //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (osIntNesting != 0u) {                       //!< See if trying to call from an ISR
        return;
    }
    if (osLockNesting != 0u) {                      //!< See if called with scheduler locked
        return;
    }
    
    if ((ticks != 0u) && (ticks != OS_INFINITE)) {  //!< 0 means no delay!
        OSEnterCriticalSection(cpu_sr);
        OS_ScheduleUnreadyTask(osTCBCur);
        OS_SchedulePendTask(osTCBCur, ticks);
        OSExitCriticalSection(cpu_sr);
        OS_Schedule();                              //!< Find next task to run!
    }
}

/*!
 *! \Brief       STATISTICS INITIALIZATION
 *!
 *! \Description This function is called by your application to establish CPU usage by first determining
 *!              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
 *!              during that time.  CPU usage is then determined by a low priority task which keeps track
 *!              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
 *!              determined by:
 *!
 *!                                             osIdleCtr
 *!                 CPU Usage (%) = 100 * (1 - ------------)
 *!                                            osIdleCtrMax
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
#if OS_STAT_EN > 0u
void osStatInit(void)
{
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    osTimeDelay(2u);                            //!< Synchronize with clock tick
    OSEnterCriticalSection(cpu_sr);
    osIdleCtr       = 0u;                       //!< Clear idle counter
    OSExitCriticalSection(cpu_sr);
    osTimeDelay(OS_TICKS_PER_SEC);              //!< Determine MAX. idle counter value for 1 second
    OSEnterCriticalSection(cpu_sr);
    osIdleCtrMax    = osIdleCtr;                //!< Store maximum idle counter count in 1 second
    osStatRunning   = TRUE;
    OSExitCriticalSection(cpu_sr);
}
#endif

/*!
 *! \Brief       CREATING THE STATISTIC TASK
 *!
 *! \Description This function creates the Statistic Task.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
#if OS_STAT_EN > 0u
static void os_init_statistics_task(void)
{
    (void)osTaskCreate(NULL,
                       os_task_statistics,
                       NULL,                                     //!< No args passed to os_task_statistics()*/
                       OS_TASK_STAT_PRIO,                           //!< One higher than the idle task
                       osTaskStatStk,                               //!< Set Bottom-Of-Stack
                       OS_TASK_STAT_STK_SIZE,
                       OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  //!< Enable stack checking + clear
}
#endif

/*!
 *! \Brief       CREATING THE IDLE TASK
 *!
 *! \Description This function creates the Idle Task.
 *!
 *! \Arguments   none
 *!
 *! \Returns     none
 */
static void os_init_idle_task(void)
{
    (void)osTaskCreate(NULL,
                       os_task_idle,
                       NULL,                                     //!< No arguments passed to os_task_idle()
                       OS_TASK_IDLE_PRIO,                           //!< Lowest priority level
                       osTaskIdleStk,                               //!< Set Bottom-Of-Stack
                       OS_TASK_IDLE_STK_SIZE,
                       OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  //!< Enable stack checking + clear stack
}

/*!
 *! \Brief       IDLE TASK
 *!
 *! \Description This task is internal to OS and executes whenever no other higher priority tasks
 *!              executes because they are ALL waiting for event(s) to occur.
 *!
 *! \Arguments   parg     this pointer is not used at this time.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) OSTaskIdleHook() is called after the critical section to ensure that interrupts will be
 *!                 enabled for at least a few instructions.  On some processors (ex. Philips XA), enabling
 *!                 and then disabling interrupts didn't allow the processor enough time to have interrupts
 *!                 enabled before they were disabled again.  OS would thus never recognize
 *!                 interrupts.
 *!              2) This hook has been added to allow you to do such things as STOP the CPU to conserve
 *!                 power.
 */
static void os_task_idle(void *parg)
{
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    parg = parg;                               //!< Prevent compiler warning for not using 'parg'
    
    for (;;) {
        OSEnterCriticalSection(cpu_sr);
        osIdleCtr++;
        OSExitCriticalSection(cpu_sr);
        
#if OS_CPU_HOOKS_EN > 0u
        OSTaskIdleHook();                        //!< Call user definable HOOK
#endif
    }
}

/*!
 *! \Brief       STATISTICS TASK
 *!
 *! \Description This task is internal to OS and is used to compute some statistics about the
 *!              multitasking environment.  Specifically, os_task_statistics() computes the CPU usage.
 *!              CPU usage is determined by:
 *!
 *!                                          osIdleCtr
 *!                 osCPUUsage = 100 * (1 - ------------)     (units are in %)
 *!                                         osIdleCtrMax
 *!
 *! \Arguments   parg     this pointer is not used at this time.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
 *!                 next higher priority, OS_TASK_IDLE_PRIO-1.
 *!              2) You can disable this task by setting the configuration #define OS_STAT_EN to 0.
 *!              3) You MUST have at least a delay of 1 seconds to allow for the system to establish the
 *!                 maximum value for the idle counter.
 */
#if OS_STAT_EN > 0u
static void os_task_statistics(void *parg)
{
    UINT32          idl_cnt;                        //!< Val. reached by idle ctr at run time in 1 sec.
    OS_LIST_NODE   *list;
    UINT8           i;
    OS_TCB         *ptcb;
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    parg = parg;                                //!< Prevent compiler warning for not using 'parg'
    
    while (osStatRunning == FALSE) {         //!< Wait until osIdleCtrMax has been measured.
        osTimeDelay(OS_TICKS_PER_SEC);
    }
    osIdleCtrMax /= 100u;
    if (osIdleCtrMax == 0u) {
        osCPUUsage = 0u;
        osTimeDelay(OS_INFINITE);
    }
    
    OSEnterCriticalSection(cpu_sr);                        //!< Initial CPU usage as 0%
    osIdleCtr = osIdleCtrMax * 100u;
    OSExitCriticalSection(cpu_sr);
    
    for (;;) {
        OSEnterCriticalSection(cpu_sr);
        idl_cnt     = osIdleCtr;                //!< Obtain the of the idle counter for the past second
        osIdleCtr   = 0u;                       //!< Reset the idle counter for the next second
        OSExitCriticalSection(cpu_sr);
        osCPUUsage  = 100u - idl_cnt / osIdleCtrMax;
        
#if (OS_STAT_TASK_STK_CHK_EN > 0u)
        for (list = osPndList.Next; list != &osPndList; list = list->Next) {
            ptcb = OS_CONTAINER_OF(list, OS_TCB, OSTCBList);
            OS_TaskStkChk(ptcb);
        }
        for (i = 0; i < OS_BITMAP_TBL_SIZE; i++) {
            for (list = osRdyList[i].Next; list != &osRdyList[i]; list = list->Next) {
                ptcb = OS_CONTAINER_OF(list, OS_TCB, OSTCBList);
                OS_TaskStkChk(ptcb);
            }
        }
#endif
        
#if OS_CPU_HOOKS_EN > 0u
        OSTaskStatHook();                       //!< Invoke user definable hook
#endif
        
        osTimeDelay(OS_TICKS_PER_SEC);          //!< Accumulate osIdleCtr for the next 1/10 second
    }
}
#endif

/*!
 *! \Brief       INITIALIZE TCB
 *!
 *! \Description This function is internal to OS and is used to get and initialize a Task Control Block when
 *!              a task is created (see osTaskCreate()).
 *!
 *! \Arguments   prio          is the priority of the task being created
 *!
 *!              psp           stack pointer when task begin to run. This value return by OSTaskStkInit.
 *!
 *!              pstk          Point to the LOWEST (valid) memory location of the stack. It's not stack
 *!                            pointer.
 *!
 *!              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
 *!                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
 *!                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
 *!                            units are established by the #define constant OS_STK which is CPU
 *!                            specific.  'stk_size' is 0 if called by 'osTaskCreate()'.
 *!
 *!              opt           options as passed to 'osTaskCreate()' or,
 *!                            0 if called from 'osTaskCreate()'.
 *!
 *! \Returns     none
 *!
 *! \Note        This function is INTERNAL to OS and your application should not call it.
 */
void OS_TCBInit(OS_TCB  *ptcb,
                UINT8    prio,
                OS_STK  *psp,
                OS_STK  *pstk,
                UINT32   stk_size,
                UINT8    opt)
{
    ptcb->OSTCBOpt          = opt;                      //!< Store task options
    ptcb->OSTCBPrio         = prio;                     //!< Load task priority into TCB
    ptcb->OSTCBStkPtr       = psp;                      //!< Load Stack Pointer in TCB

    os_list_init_head(&ptcb->OSTCBList);
    ptcb->OSTCBWaitNode     = NULL;                     //!< Task is not pending on anay object.
    ptcb->OSTCBOwnMutex     = NULL;
    
    ptcb->OSTCBDly          = 0u;                       //!< Task is not delayed
    ptcb->OSTCBTimeSlice    = 0u;

#if OS_TASK_PROFILE_EN > 0u                             //!< Initialize profiling variables
    ptcb->OSTCBStkBase      = pstk;
    ptcb->OSTCBStkSize      = stk_size;
    ptcb->OSTCBStkUsed      = 0u;
    ptcb->OSTCBCtxSwCtr     = 0u;
    ptcb->OSTCBCyclesStart  = 0u;
    ptcb->OSTCBCyclesTot    = 0u;
#endif

#if OS_CPU_HOOKS_EN > 0u
    OSTCBInitHook(ptcb);
#endif
}

/*!
 *! \Brief       STACK CHECKING
 *!
 *! \Description This function is called to check the amount of free memory left on the specified task's
 *!              stack.
 *!
 *! \Arguments   prio          is the task priority
 *!
 *! \Returns     OS_ERR_NONE            upon success
 *!              OS_ERR_INVALID_PRIO    if the priority you specify is higher that the maximum allowed
 *!              OS_ERR_TASK_NOT_EXIST  if the desired task has not been created or is assigned to a Mutex PIP
 *!              OS_ERR_TASK_OPT        if you did NOT specified OS_TASK_OPT_STK_CHK when the task was created
 *!              OS_ERR_PDATA_NULL      if 'p_stk_data' is a NULL pointer
 */
#if (OS_STAT_TASK_STK_CHK_EN > 0u)
void OS_TaskStkChk(OS_TCB *ptcb)
{
    OS_STK    *pstk;
    UINT32     nfree;
    UINT32     size;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    OSEnterCriticalSection(cpu_sr);
    if ((ptcb->OSTCBOpt & OS_TASK_OPT_STK_CHK) == 0u) { //!< Make sure stack checking option is set
        OSExitCriticalSection(cpu_sr);
        return;
    }
    size  = ptcb->OSTCBStkSize;
    pstk  = ptcb->OSTCBStkBase;
#if OS_STK_GROWTH_DOWN == 1u
#else
    pstk += size;
#endif

    nfree = 0u;
#if OS_STK_GROWTH_DOWN == 1u
    while (*pstk++ == 0u) {                             //!< Compute the number of zero entries on the stk
        nfree++;
    }
#else
    while (*pstk-- == 0u) {
        nfree++;
    }
#endif
    ptcb->OSTCBStkUsed = size - nfree;                  //!< Compute number of entries used on the stk
    OSExitCriticalSection(cpu_sr);
}
#endif

/*!
 *! \Brief       MAKE TASK WAIT FOR EVENT TO OCCUR
 *!
 *! \Description This function is called by other OS services to suspend a task because an event has
 *!              not occurred.
 *!
 *! \Arguments   pevent   is a pointer to the event control block for which the task will be waiting for.
 *!
 *!              pnode    is a pointer to a structure which contains data about the task waiting for
 *!                       event to occur.
 *!
 *!              ticks    is the desired amount of ticks that the task will wait for the event to
 *!                       occur.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function assumes that interrupts are disabled.
 *!              2) This function is INTERNAL to OS and your application should not call it.
 */
void OS_EventTaskWait(  void           *pecb,
                        OS_WAIT_NODE   *pnode,
                        UINT32          ticks)
{
    OS_WAITBALE_OBJ    *pobj = (OS_WAITBALE_OBJ *)pecb;
    OS_LIST_NODE       *list;
#if OS_CRITICAL_METHOD == 3u            //!< Allocate storage for CPU status register
    OS_CPU_SR     cpu_sr = 0u;
#endif


    OSEnterCriticalSection(cpu_sr);
    os_list_init_head(&pnode->OSWaitNodeList);
    pnode->OSWaitNodeTCB    = osTCBCur;             //!< Link to task's TCB
    pnode->OSWaitNodeECB    = pecb;                 //!< Link to node
    pnode->OSWaitNodeRes    = OS_STAT_PEND_OK;

    osTCBCur->OSTCBWaitNode = pnode;                    //!< Store node in task's TCB
    if (OS_OBJ_PRIO_TYPE_GET(pobj->OSObjType) == OS_OBJ_PRIO_TYPE_PRIO_LIST) {
        OS_WAIT_NODE* nextNode;
        
        for (list = pobj->OSWaitObjWaitList.Next; list != &pobj->OSWaitObjWaitList; list = list->Next) {
            nextNode = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
            if (osTCBCur->OSTCBPrio < nextNode->OSWaitNodeTCB->OSTCBPrio) {
                break;
            }
        }
    } else {
        list = &pobj->OSWaitObjWaitList;
    }
    os_list_add(&pnode->OSWaitNodeList, list->Prev);    //!< add wait node to the end of wait NODE list.

    OS_ScheduleUnreadyTask(osTCBCur);
    OS_SchedulePendTask(osTCBCur, ticks);
    OSExitCriticalSection(cpu_sr);
}

/*!
 *! \Brief       REMOVE TASK FROM EVENT WAIT LIST
 *!
 *! \Description Remove a task from an event's wait list.
 *!
 *! \Arguments   ptcb     is a pointer to the task to remove.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function assumes that interrupts are disabled.
 *!              2) This function is INTERNAL to OS and your application should not call it.
 */
void OS_EventTaskRemove(OS_TCB *ptcb)
{
    OS_WAIT_NODE *pnode = ptcb->OSTCBWaitNode;
    

    os_list_del(&pnode->OSWaitNodeList);    //!< remove from wait NODE list.
    ptcb->OSTCBWaitNode = NULL;
    OS_ScheduleUnpendTask(ptcb);
}

/*!
 *! \Brief       MAKE TASK READY TO RUN BASED ON EVENT OCCURING
 *!
 *! \Description This function is called by other OS services and is used to make a task ready-to-run because
 *!              desired event occur.
 *!
 *! \Arguments   pevent      is a pointer to the event control block corresponding to the event.
 *!
 *!              pendRes   is used to indicate the readied task's pending status:
 *!
 *!                          OS_STAT_PEND_OK      Task ready due to a event-set, not a timeout or
 *!                                               an abort.
 *!                          OS_STAT_PEND_ABORT   Task ready due to an abort(or event was deleted).
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function assumes that interrupts are DISABLED.
 *!              2) This function is INTERNAL to OS and your application should not call it.
 */
OS_TCB *OS_EventTaskRdy(void   *pecb,
                        UINT8   pendRes)
{
    OS_WAITBALE_OBJ    *pobj = (OS_WAITBALE_OBJ*)pecb;
    OS_WAIT_NODE       *pnode;
    OS_TCB             *ptcb;

    
    pnode   = OS_CONTAINER_OF(pobj->OSWaitObjWaitList.Next, OS_WAIT_NODE, OSWaitNodeList);
    ptcb    = pnode->OSWaitNodeTCB;
        
    ptcb->OSTCBDly       = 0u;
    pnode->OSWaitNodeRes = pendRes;
    OS_EventTaskRemove(ptcb);                           //!< Remove this task from event's wait list
    OS_ScheduleReadyTask(ptcb);                         //!< Put task in the ready list
    return ptcb;
}

/*!
 *! \Brief       CLEAR A SECTION OF MEMORY
 *!
 *! \Description This function is called by other OS services to clear a contiguous block of RAM.
 *!
 *! \Arguments   pdest    is the start of the RAM to clear (i.e. write 0x00 to)
 *!
 *!              size     is the number of bytes to clear.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function is INTERNAL to OS and your application should not call it.
 *!              2) Note that we can only clear up to 0xFFFFFFFF bytes of RAM at once.
 *!              3) The clear is done one byte at a time since this will work on any processor irrespective
 *!                 of the alignment of the destination.
 */
void OS_MemClr(UINT8 *pdest, UINT32 size)
{
    while (size > 0u) {
        *pdest++ = 0u;
        size--;
    }
}

/*!
 *! \Brief       COPY A BLOCK OF MEMORY
 *!
 *! \Description This function is called by other OS services to copy a block of memory from one
 *!              location to another.
 *!
 *! \Arguments   pdest    is a pointer to the 'destination' memory block
 *!
 *!              psrc     is a pointer to the 'source'      memory block
 *!
 *!              size     is the number of bytes to copy.
 *!
 *! \Returns     none
 *!
 *! \Notes       1) This function is INTERNAL to OS and your application should not call it.  There is
 *!                 no provision to handle overlapping memory copy.  However, that's not a problem since this
 *!                 is not a situation that will happen.
 *!              2) Note that we can only copy up to 0xFFFFFFFF bytes of RAM at once.
 *!              3) The copy is done one byte at a time since this will work on any processor irrespective
 *!                 of the alignment of the source and destination.
 */
void OS_MemCopy(UINT8 *pdest, UINT8 *psrc, UINT32 size)
{
    while (size > 0u) {
        *pdest++ = *psrc++;
        size--;
    }
}

/*!
 *! \Brief       GET VERSION
 *!
 *! \Description This function is used to return the version number of OS.  The returned value
 *!              corresponds to OS's version number multiplied by 10000.  In other words, version
 *!              2.01.00 would be returned as 20100.
 *!
 *! \Arguments   none
 *!
 *! \Returns     The version number of OS multiplied by 10000.
 */
UINT16 osVersion(void)
{
    return OS_VERSION;
}

