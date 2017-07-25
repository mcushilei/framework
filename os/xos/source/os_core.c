
//! \note do not move this pre-processor statement to other places
#define  __OS_CORE_C__

/*============================ INCLUDES ======================================*/
#include ".\os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
static  void    os_init_free_obj_list(void);

static  void    os_init_misc(void);

#if OS_STAT_EN > 0u
static  void    os_init_statistics_task(void);
static  void    os_task_statistics(void *parg);
#endif

static  void    os_init_idle_task(void);
static  void    os_task_idle(void *parg);

/*============================ LOCAL VARIABLES ===============================*/
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

    os_init_misc();                                              //!< Initialize miscellaneous variables

    os_init_free_obj_list();                                           //!< Initialize the free list of OS_TCBs

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
    
    os_list_init_head(&osSleepList);
    os_list_init_head(&osWaitableObjList);
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


static void os_init_free_obj_list(void)
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
 *! \Note        OSStartTheFirst() MUST:
 *!                 a) Call OSTaskSwHook() then,
 *!                 b) Set osRunning to TRUE.
 *!                 c) Load the context of the task pointed to by osTCBNextRdy.
 *!                 d) Execute the task.
 *!                 e) Enable system-level interrupt.
 */
void osStart(void)
{
    if (osRunning == FALSE) {           //!< os must not be running!
        OS_ScheduleNext();
        osTCBCur = osTCBNextRdy;
        OSStartTheFirst();               //!< Execute target specific code to start task
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
    OS_LIST_NODE       *list, *listObj;
    OS_TCB             *ptcb;
    OS_WAITBALE_OBJ    *pobj;
    OS_WAIT_NODE       *pnode;
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    if (osRunning == FALSE) {
        return;
    }
    
#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
    OSTimeTickHook();
#endif
    
    for (listObj = osWaitableObjList.Next; listObj != &osWaitableObjList; listObj = listObj->Next) {
        pobj = OS_CONTAINER_OF(listObj, OS_WAITBALE_OBJ, OSWaitObjList);
        //! if this objcet is not locked.
        for (list = pobj->OSWaitObjWaitNodeList.Next; list != &pobj->OSWaitObjWaitNodeList; ) {   //!< Go through all task in TCB list.
            pnode  = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
            list = list->Next;
            ptcb = pnode->OSWaitNodeTCB;
            
            if (pnode->OSWaitNodeDly != 0u) {
                pnode->OSWaitNodeDly--;
                if (pnode->OSWaitNodeDly == 0u) {                   //!< If timeout
                    pnode->OSWaitNodeRes = OS_STAT_PEND_TO;         //!< Indicate PEND timeout.
                    OS_WaitNodeRemove(ptcb);
                    OSEnterCriticalSection(cpu_sr);
                    OS_ScheduleReadyTask(ptcb);
                    OSExitCriticalSection(cpu_sr);
                }
            }
        }
    }
    for (list = osSleepList.Next; list != &osSleepList; ) {         //!< Go through all task in sleep TCB list.
        pnode  = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
        list = list->Next;
        ptcb = pnode->OSWaitNodeTCB;
        
        if (pnode->OSWaitNodeDly != 0u) {
            pnode->OSWaitNodeDly--;
            if (pnode->OSWaitNodeDly == 0u) {                       //!< If timeout
                OS_WaitNodeRemove(ptcb);
                OSEnterCriticalSection(cpu_sr);
                OS_ScheduleReadyTask(ptcb);
                OSExitCriticalSection(cpu_sr);
            }
        }
    }
}

void OS_SleepTask(  OS_TCB         *ptcb,
                    OS_WAIT_NODE   *pnode,
                    UINT32          ticks)
{
    if (ticks == OS_INFINITE) {
        ticks = 0u;
    }
    ptcb->OSTCBWaitNode     = pnode;
    pnode->OSWaitNodeTCB    = ptcb;
    pnode->OSWaitNodeDly    = ticks;
    
    os_list_add(&pnode->OSWaitNodeList, osSleepList.Prev);  //!< add task to the end of PEND task list.
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
 *! \Notes       1) This function assumes that interrupts are DISABLED.
 *!              2) This function is INTERNAL to OS and your application should not call it.
 */
void OS_WaitNodeRemove(OS_TCB *ptcb)
{
    OS_WAIT_NODE *pnode = ptcb->OSTCBWaitNode;
    

    os_list_del(&pnode->OSWaitNodeList);    //!< remove from wait NODE list.
    ptcb->OSTCBWaitNode = NULL;
}

/*!
 *! \Brief       SEND TASK TO SLEEP FOR 'n' TICKS
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
void osTaskSleep(UINT32 ticks)
{
    OS_WAIT_NODE    node;
#if OS_CRITICAL_METHOD == 3u                        //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    if (osIntNesting != 0u) {                       //!< See if trying to call from an ISR
        return;
    }
    if (osLockNesting != 0u) {                      //!< See if called with scheduler locked
        return;
    }
    
    if ((ticks == 0u) && (ticks == OS_INFINITE)) {  //!< 0 means no delay!
        return;
    }
    
    OSEnterCriticalSection(cpu_sr);
    OS_ScheduleUnreadyTask(osTCBCur);
    OS_SleepTask(osTCBCur, &node, ticks);
    OSExitCriticalSection(cpu_sr);
    OS_Schedule();                              //!< Find next task to run!
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


    osTaskSleep(2u);                            //!< Synchronize with clock tick
    OSEnterCriticalSection(cpu_sr);
    osIdleCtr       = 0u;                       //!< Clear idle counter
    OSExitCriticalSection(cpu_sr);
    osTaskSleep(OS_TICKS_PER_SEC);              //!< Determine MAX. idle counter value for 1 second
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
    OS_LIST_NODE       *list, *listObj;
    OS_TCB             *ptcb;
    OS_WAITBALE_OBJ    *pobj;
    OS_WAIT_NODE       *pnode;
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR       cpu_sr = 0u;
#endif


    parg = parg;                                //!< Prevent compiler warning for not using 'parg'
    
    while (osStatRunning == FALSE) {         //!< Wait until osIdleCtrMax has been measured.
        osTaskSleep(OS_TICKS_PER_SEC);
    }
    osIdleCtrMax /= 100u;
    if (osIdleCtrMax == 0u) {
        osCPUUsage = 0u;
        osTaskSleep(OS_INFINITE);
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
        OSEnterCriticalSection(cpu_sr);
        for (list = osSleepList.Next; list != &osSleepList; list = list->Next) {
            pnode = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
            ptcb  = pnode->OSWaitNodeTCB;
            OS_TaskStkChk(ptcb);
        }
        for (listObj = osWaitableObjList.Next; listObj != &osWaitableObjList; listObj = listObj->Next) {
            pobj = OS_CONTAINER_OF(listObj, OS_WAITBALE_OBJ, OSWaitObjList);
            for (list = pobj->OSWaitObjWaitNodeList.Next; list != &pobj->OSWaitObjWaitNodeList; list = list->Next) {   //!< Go through all task in TCB list.
                pnode  = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
                ptcb = pnode->OSWaitNodeTCB;
                OS_TaskStkChk(ptcb);
            }
        }
        OSExitCriticalSection(cpu_sr);
#endif
        
#if OS_CPU_HOOKS_EN > 0u
        OSTaskStatHook();                       //!< Invoke user definable hook
#endif
        
        osTaskSleep(OS_TICKS_PER_SEC);          //!< Accumulate osIdleCtr for the next 1/10 second
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
    
    ptcb->OSTCBTimeSlice    = 0u;
    ptcb->OSTCBTimeSliceCnt = 0u;

    ptcb->OSTCBStkPtr       = psp;                      //!< Load Stack Pointer in TCB
    os_list_init_head(&ptcb->OSTCBList);
    ptcb->OSTCBWaitNode     = NULL;                     //!< Task is not pending on anay object.
    ptcb->OSTCBOwnMutex     = NULL;

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

void OS_RegWaitableObj(OS_WAITBALE_OBJ *pobj)
{
    os_list_add(&pobj->OSWaitObjList, osWaitableObjList.Prev);    //!< and object at the end of list.
}

void OS_DeregWaitableObj(OS_WAITBALE_OBJ *pobj)
{
    os_list_del(&pobj->OSWaitObjList);          //!< remove object from list.
}

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
 *! \Notes       1) This function assumes that interrupts are DISABLED.
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
    if (ticks == OS_INFINITE) {
        ticks = 0u;
    }
    pnode->OSWaitNodeDly = ticks;

    osTCBCur->OSTCBWaitNode = pnode;                    //!< Store node in task's TCB
    if (OS_OBJ_PRIO_TYPE_GET(pobj->OSObjType) == OS_OBJ_PRIO_TYPE_PRIO_LIST) {
        OS_WAIT_NODE *nextNode;
        
        for (list = pobj->OSWaitObjWaitNodeList.Next; list != &pobj->OSWaitObjWaitNodeList; list = list->Next) {
            nextNode = OS_CONTAINER_OF(list, OS_WAIT_NODE, OSWaitNodeList);
            if (osTCBCur->OSTCBPrio < nextNode->OSWaitNodeTCB->OSTCBPrio) {
                break;
            }
        }
    } else {
        list = &pobj->OSWaitObjWaitNodeList;
    }
    os_list_add(&pnode->OSWaitNodeList, list->Prev);    //!< add wait node to the end of wait NODE list.

    OS_ScheduleUnreadyTask(osTCBCur);
    OSExitCriticalSection(cpu_sr);
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

    
    pnode   = OS_CONTAINER_OF(pobj->OSWaitObjWaitNodeList.Next, OS_WAIT_NODE, OSWaitNodeList);
    ptcb    = pnode->OSWaitNodeTCB;
        
    pnode->OSWaitNodeRes = pendRes;
    OS_WaitNodeRemove(ptcb);                           //!< Remove this task from event's wait list
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

