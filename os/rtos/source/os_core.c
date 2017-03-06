
#define  OS_GLOBALS
#include ".\os.h"

/*********************************************************************************************************
*                                      PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*       Leading 0 algorithm.
*********************************************************************************************************/

static INT8U const  OSUnMapTbl[256] = {
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

/*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************/

static  void    OS_InitEventList(void);

static  void    OS_InitTCBList(void);

static  void    OS_InitMisc(void);

static  void    OS_InitRdyTable(void);

static  INT8U   OS_PrioGetHighest(void);

#if OS_STAT_EN > 0u
static  void    OS_InitTaskStat(void);
static  void    OS_TaskStat(void *parg);
#endif

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
static  void    OS_StatTaskStkChk(void);
#endif

static  void    OS_InitTaskIdle(void);
static  void    OS_TaskIdle(void *parg);



/*********************************************************************************************************
*                                           INITIALIZATION
*
* Description: This function is used to initialize the internals of OS and MUST be called prior to
*              creating any OS object and, prior to calling osStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

void  osInit (void)
{
#if OS_CPU_HOOKS_EN > 0
    OSInitHookBegin();                                           //!< Call port specific initialization code
#endif

    OS_InitMisc();                                               //!< Initialize miscellaneous variables

    OS_InitRdyTable();                                            //!< Initialize the Ready List

    OS_InitTCBList();                                            //!< Initialize the free list of OS_TCBs

#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
    OS_InitEventList();                                          //!< Initialize the free list of OS_EVENTs
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
    OS_FlagListInit();                                           //!< Initialize the free list of event flag
#endif

    OS_InitTaskIdle();                                           //!< Create the Idle Task
#if OS_STAT_EN > 0u
    OS_InitTaskStat();                                           //!< Create the Statistic Task
#endif

#if OS_CPU_HOOKS_EN > 0u
    OSInitHookEnd();                                             //!< Call port specific init. code
#endif

#if OS_DEBUG_EN > 0u
    OSDebugInit();
#endif
}

/*********************************************************************************************************
*                                              ENTER ISR
*
* Description: This function is used to notify OS that you are about to service an interrupt
*              service routine (ISR).  This allows OS to keep track of interrupt nesting and thus
*              only perform rescheduling at the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function should be called with interrupts already DISABLED
*              2) Your ISR can directly increment OSIntNesting without calling this function because
*                 OSIntNesting has been declared 'global'.
*              3) You MUST still call osIntExit() even though you increment OSIntNesting directly.
*              4) You MUST invoke osIntEnter() and osIntExit() in pair.  In other words, for every call
*                 to osIntEnter() at the beginning of the ISR you MUST have a call to osIntExit() at the
*                 end of the ISR.
*              5) You are allowed to nest interrupts up to 255 levels deep.
*              6) I removed the OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL() around the increment because
*                 osIntEnter() is always called with interrupts disabled.
*********************************************************************************************************/

void  osIntEnter (void)
{
    if (OSRunning == OS_TRUE) {
        if (OSIntNesting < 255u) {
            OSIntNesting++;                      //!< Increment ISR nesting level
        }
    }
}

/*********************************************************************************************************
*                                              EXIT ISR
*
* Description: This function is used to notify uOS that you have completed servicing an ISR.  When
*              the last nested ISR has completed, OS will call the scheduler to determine whether
*              a new, high-priority task, is ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke osIntEnter() and osIntExit() in pair.  In other words, for every call
*                 to osIntEnter() at the beginning of the ISR you MUST have a call to osIntExit() at the
*                 end of the ISR.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************/

void  osIntExit (void)
{
    INT8U       prio;
#if OS_CRITICAL_METHOD == 3u                                //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    if (OSRunning == OS_TRUE) {
        OS_ENTER_CRITICAL();
        if (OSIntNesting > 0u) {                            //!< Prevent OSIntNesting from wrapping
            OSIntNesting--;
        }
        if (OSIntNesting == 0u) {                           //!< Reschedule only if all ISRs complete ...
            if (OSLockNesting == 0u) {                      //!< ... and not locked.
                prio = OS_PrioGetHighest();
                OSTCBHighRdy = OSTaskPrioTCBTbl[prio];
                if (OSTCBHighRdy != OSTCBCur) {             //!< No Ctx Sw if current task is highest rdy
#if OS_TASK_PROFILE_EN > 0u
                    OSTCBHighRdy->OSTCBCtxSwCtr++;          //!< Inc. # of context switches to this task
#endif
#if OS_STAT_EN > 0u
                    OSCtxSwCtr++;                           //!< Keep track of the number of ctx switches
#endif
                    OSIntCtxSw();                           //!< Perform interrupt level ctx switch
                }
            }
        }
        OS_EXIT_CRITICAL();
    }
}


/*********************************************************************************************************
*                                         PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling to take place.  This allows your application
*              to prevent context switches until you are ready to permit context switching.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke osSchedLock() and osSchedUnlock() in pair.  In other words, for every
*                 call to osSchedLock() you MUST have a call to osSchedUnlock().
*********************************************************************************************************/

#if OS_SCHED_LOCK_EN > 0u
void  osSchedLock (void)
{
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (OSRunning == OS_TRUE) {                  //!< Make sure multitasking is running
        OS_ENTER_CRITICAL();
        if (OSIntNesting == 0u) {                //!< Can't call from an ISR
            if (OSLockNesting < 255u) {          //!< Prevent OSLockNesting from wrapping back to 0
                OSLockNesting++;                 //!< Increment lock nesting level
            }
        }
        OS_EXIT_CRITICAL();
    }
}
#endif

/*********************************************************************************************************
*                                          ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke osSchedLock() and osSchedUnlock() in pair.  In other words, for every
*                 call to osSchedLock() you MUST have a call to osSchedUnlock().
*********************************************************************************************************/

#if OS_SCHED_LOCK_EN > 0u
void  osSchedUnlock (void)
{
#if OS_CRITICAL_METHOD == 3u                               //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (OSRunning == OS_TRUE) {                            //!< Make sure multitasking is running
        OS_ENTER_CRITICAL();
        if (OSIntNesting == 0u) {                          //!< Can't call from an ISR
            if (OSLockNesting > 0u) {                      //!< Do not decrement if already 0
                OSLockNesting--;                           //!< Decrement lock nesting level
                if (OSLockNesting == 0u) {                 //!< See if scheduler is enabled
                    OS_EXIT_CRITICAL();
                    OS_Sched();                            //!< See if a HPT is ready
                } else {
                    OS_EXIT_CRITICAL();
                }
            } else {
                OS_EXIT_CRITICAL();
            }
        } else {
            OS_EXIT_CRITICAL();
        }
    }
}
#endif

/*********************************************************************************************************
*                                         START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets OS manages the
*              task that you have created.  Before you can call osStart(), you MUST have called osInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to OS_TRUE.
*                 c) Load the context of the task pointed to by OSTCBHighRdy.
*                 d) Execute the task.
*                 e) Enable system-level interrupt.
*********************************************************************************************************/

void  osStart (void)
{
    INT8U   prio;
    
    
    if (OSRunning == OS_FALSE) {
        prio = OS_PrioGetHighest();
        OSTCBHighRdy  = OSTaskPrioTCBTbl[prio];   //!< Point to highest priority task ready to run
        OSTCBCur      = OSTCBHighRdy;
        OSStartHighRdy();                           //!< Execute target specific code to start task
    }
}

/*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*
* Description: This function is used to signal to OS the occurrence of a 'system tick' (also known
*              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
*              called by a HIGH priority task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

void  osTimeTick (void)
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u                                //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
    OSTimeTickHook();                                       //!< Call user definable hook
#endif
    
    if (OSRunning == OS_TRUE) {
        ptcb = OSTaskList;                             //!< Point at first TCB in TCB list
        while (ptcb->OSTCBPrio != OS_TASK_IDLE_PRIO) {      //!< Go through all TCBs in TCB list...
            OS_ENTER_CRITICAL();                            //!<   Idle task is always the first task created,
            if ((ptcb->OSTCBDly != 0u) &&                   //!<   so, it's always at the end of the list.*/
                (ptcb->OSTCBDly != OS_INFINITE)) {          //!< No, Delayed or waiting for event with TO
                ptcb->OSTCBDly--;                           //!< Decrement nbr of ticks to end of delay
                if (ptcb->OSTCBDly == 0u) {                 //!< Check for timeout

                    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY) {  //!< See if waiting for any event
                        ptcb->OSTCBStat    &= (INT8U)~(INT8U)OS_STAT_PEND_ANY;  //!< Yes, Clear status flag
                        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;                  //!< Indicate PEND timeout
                    } else {
                        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;
                    }

                    OSRdyGrp               |= ptcb->OSTCBBitY;                  //!< Make task ready
                    OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                }
            }
            ptcb = ptcb->OSTCBNext;                         //!< Point at next TCB in TCB list
            OS_EXIT_CRITICAL();
        }
    }
}

/*********************************************************************************************************
*                                        DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************/

void  osTimeDelay(INT32U ticks)
{
    INT8U      y;
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    if (OSIntNesting != 0u) {                     //!< See if trying to call from an ISR
        return;
    }
    if (OSLockNesting != 0u) {                    //!< See if called with scheduler locked
        return;
    }
    if (ticks > 0u) {                            //!< 0 means no delay!
        OS_ENTER_CRITICAL();
        y = OSTCBCur->OSTCBY;                    //!< remove task from ready table
        OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
        if (OSRdyTbl[y] == 0u) {
            OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
        }
        OSTCBCur->OSTCBDly = ticks;              //!< Load ticks in TCB
        OS_EXIT_CRITICAL();
        OS_Sched();                              //!< Find next task to run!
    }
}

/*********************************************************************************************************
*                                      STATISTICS INITIALIZATION
*
* Description: This function is called by your application to establish CPU usage by first determining
*              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
*              during that time.  CPU usage is then determined by a low priority task which keeps track
*              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
*              determined by:
*
*                                             OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------)
*                                            OSIdleCtrMax
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

#if OS_STAT_EN > 0u
void  osStatInit (void)
{
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    osTimeDelay(2u);                            //!< Synchronize with clock tick
    OS_ENTER_CRITICAL();
    OSIdleCtr       = 0u;                       //!< Clear idle counter
    OS_EXIT_CRITICAL();
    osTimeDelay(OS_TICKS_PER_SEC);              //!< Determine MAX. idle counter value for 1 second
    OS_ENTER_CRITICAL();
    OSIdleCtrMax    = OSIdleCtr;                //!< Store maximum idle counter count in 1 second
    OSStatRunning   = OS_TRUE;
    OS_EXIT_CRITICAL();
}
#endif

/*********************************************************************************************************
*                                             GET VERSION
*
* Description: This function is used to return the version number of OS.  The returned value
*              corresponds to OS's version number multiplied by 10000.  In other words, version
*              2.01.00 would be returned as 20100.
*
* Arguments  : none
*
* Returns    : The version number of OS multiplied by 10000.
*********************************************************************************************************/

INT16U  osVersion (void)
{
    return OS_VERSION;
}

/*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*
* Description: This function is called by osInit() to initialize miscellaneous variables.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

static  void  OS_InitMisc (void)
{
    OSIntNesting             = 0u;                        //!< Clear the interrupt nesting counter
    OSLockNesting            = 0u;                        //!< Clear the scheduling lock counter

    OSTaskCtr                = 0u;                        //!< Clear the number of tasks

    OSRunning                = OS_FALSE;                  //!< Indicate that multitasking not started

    OSIdleCtr                = 0u;                        //!< Clear the idle counter

#if OS_STAT_EN > 0u
    OSCtxSwCtr               = 0u;                        //!< Clear the context switch counter
    OSCPUUsage               = 0u;
    OSIdleCtrMax             = 0u;
    OSStatRunning            = OS_FALSE;                  //!< Statistic task is not ready
#endif
}

/*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*
* Description: This function is called by osInit() to initialize the Ready List.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

static  void  OS_InitRdyTable   (void)
{
    uint_fast8_t  i;


    OSRdyGrp      = 0u;
    for (i = 0u; i < OS_RDY_TBL_SIZE; i++) {
        OSRdyTbl[i] = 0u;
    }

    OSTCBCur      = OS_NULL;
    OSTCBHighRdy  = OS_NULL;
}

/*********************************************************************************************************
*                                              SCHEDULER
*
* Description: This function is called by other OS services to determine whether a new, high
*              priority task has been made ready to run.  This function is invoked by TASK level code
*              and is not used to reschedule tasks from ISRs (see osIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to OS and your application should not call it.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************/

void    OS_Sched    (void)
{
    INT8U       prio;
#if OS_CRITICAL_METHOD == 3u                            //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    OS_ENTER_CRITICAL();
    if (OSIntNesting == 0u) {                           //!< Schedule only if all ISRs done and ...
        if (OSLockNesting == 0u) {                      //!< ... scheduler is not locked
            prio = OS_PrioGetHighest();                             //!< Get the next task
            OSTCBHighRdy = OSTaskPrioTCBTbl[prio];
            if (OSTCBHighRdy != OSTCBCur) {             //!< No Ctx Sw if current task is highest rdy
#if OS_TASK_PROFILE_EN > 0u
                OSTCBHighRdy->OSTCBCtxSwCtr++;          //!< Inc. # of context switches to this task
#endif
#if OS_STAT_EN > 0u
                OSCtxSwCtr++;                           //!< Increment context switch counter
#endif
                OSCtxSw();                              //!< Perform a context switch
            }
        }
    }
    OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
*                               FIND HIGHEST PRIORITY TASK READY TO RUN
*
* Description: This function is called by other OS services to determine the highest priority task
*              that is ready to run.
*
* Arguments  : none
*
* Returns    : The priority of the highest ready task in ready table.
*
* Notes      : 1) This function is INTERNAL to OS and your application should not call it.
*              2) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************/

static  INT8U   OS_PrioGetHighest (void)
{
    INT8U   y;
    INT8U   prio;
    OS_PRIO tbl_val;


#if OS_MAX_PRIO_LEVELS <= 64u                        //!< See if we support up to 64 tasks
    y       = OSUnMapTbl[OSRdyGrp];
    tbl_val = OSRdyTbl[y];
    prio    = (y << 3u) + OSUnMapTbl[tbl_val];
#else                                            //!< We support up to 256 tasks
    if ((OSRdyGrp & 0xFFu) != 0u) {
        y = OSUnMapTbl[OSRdyGrp & 0xFFu];
    } else {
        y = OSUnMapTbl[(OSRdyGrp >> 8u) & 0xFFu] + 8u;
    }
    tbl_val = OSRdyTbl[y];
    if ((tbl_val & 0xFFu) != 0u) {
        prio = (y << 4u) + OSUnMapTbl[tbl_val & 0xFFu];
    } else {
        prio = (y << 4u) + OSUnMapTbl[(tbl_val >> 8u) & 0xFFu] + 8u;
    }
#endif
    
    return prio;
}

/*********************************************************************************************************
*                                             INITIALIZATION
*                                         CREATING THE IDLE TASK
*
* Description: This function creates the Idle Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

static  void  OS_InitTaskIdle (void)
{
    (void)osTaskCreate(OS_TaskIdle,
                       OS_NULL,                                     //!< No arguments passed to OS_TaskIdle()
                       OS_TASK_IDLE_PRIO,                           //!< Lowest priority level
                       OSTaskIdleStk,                               //!< Set Bottom-Of-Stack
                       OS_TASK_IDLE_STK_SIZE,
                       OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  //!< Enable stack checking + clear stack
}

/*********************************************************************************************************
*                                             INITIALIZATION
*                                      CREATING THE STATISTIC TASK
*
* Description: This function creates the Statistic Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

#if OS_STAT_EN > 0u
static  void  OS_InitTaskStat (void)
{
    (void)osTaskCreate(OS_TaskStat,
                       OS_NULL,                                     //!< No args passed to OS_TaskStat()*/
                       OS_TASK_STAT_PRIO,                           //!< One higher than the idle task
                       OSTaskStatStk,                               //!< Set Bottom-Of-Stack
                       OS_TASK_STAT_STK_SIZE,
                       OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  //!< Enable stack checking + clear
}
#endif

/*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to OS and executes whenever no other higher priority tasks
*              executes because they are ALL waiting for event(s) to occur.
*
* Arguments  : parg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) OSTaskIdleHook() is called after the critical section to ensure that interrupts will be
*                 enabled for at least a few instructions.  On some processors (ex. Philips XA), enabling
*                 and then disabling interrupts didn't allow the processor enough time to have interrupts
*                 enabled before they were disabled again.  OS would thus never recognize
*                 interrupts.
*              2) This hook has been added to allow you to do such things as STOP the CPU to conserve
*                 power.
*********************************************************************************************************/

static  void    OS_TaskIdle (void *parg)
{
#if OS_CRITICAL_METHOD == 3u                     //!< Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr = 0u;
#endif


    parg = parg;                               //!< Prevent compiler warning for not using 'parg'
    
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
        
#if OS_CPU_HOOKS_EN > 0u
        OSTaskIdleHook();                        //!< Call user definable HOOK
#endif
    }
}

/*********************************************************************************************************
*                                           STATISTICS TASK
*
* Description: This task is internal to OS and is used to compute some statistics about the
*              multitasking environment.  Specifically, OS_TaskStat() computes the CPU usage.
*              CPU usage is determined by:
*
*                                          OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                         OSIdleCtrMax
*
* Arguments  : parg     this pointer is not used at this time.
*
* Returns    : none
*
* Notes      : 1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
*                 next higher priority, OS_TASK_IDLE_PRIO-1.
*              2) You can disable this task by setting the configuration #define OS_STAT_EN to 0.
*              3) You MUST have at least a delay of 1 seconds to allow for the system to establish the
*                 maximum value for the idle counter.
*********************************************************************************************************/

#if OS_STAT_EN > 0u
static  void    OS_TaskStat (void *parg)
{
    INT32U      idl_cnt;                        //!< Val. reached by idle ctr at run time in 1 sec.
#if OS_CRITICAL_METHOD == 3u                    //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    parg = parg;                                //!< Prevent compiler warning for not using 'parg'
    
    while (OSStatRunning == OS_FALSE) {         //!< Wait until OSIdleCtrMax has been measured.
        osTimeDelay(OS_TICKS_PER_SEC);
    }
    OSIdleCtrMax /= 100u;
    if (OSIdleCtrMax == 0u) {
        OSCPUUsage = 0u;
        osTimeDelay(OS_INFINITE);
    }
    
    OS_ENTER_CRITICAL();                        //!< Initial CPU usage as 0%
    OSIdleCtr = OSIdleCtrMax * 100u;
    OS_EXIT_CRITICAL();
    
    for (;;) {
        OS_ENTER_CRITICAL();
        idl_cnt     = OSIdleCtr;                //!< Obtain the of the idle counter for the past second
        OSIdleCtr   = 0u;                       //!< Reset the idle counter for the next second
        OS_EXIT_CRITICAL();
        OSCPUUsage  = 100u - idl_cnt / OSIdleCtrMax;
        
#if (OS_STAT_TASK_STK_CHK_EN > 0u)
        OS_StatTaskStkChk();                    //!< Check the stacks for each task
#endif
        
#if OS_CPU_HOOKS_EN > 0u
        OSTaskStatHook();                       //!< Invoke user definable hook
#endif
        
        osTimeDelay(OS_TICKS_PER_SEC);          //!< Accumulate OSIdleCtr for the next 1/10 second
    }
}
#endif

/*********************************************************************************************************
*                                        CHECK ALL TASK STACKS
*
* Description: This function is called by OS_TaskStat() to check the stacks of each task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
static  void    OS_StatTaskStkChk   (void)
{
    uint_fast8_t prio;


    for (prio = 0u; prio <= OS_TASK_IDLE_PRIO; prio++) {
        OS_TaskStkChk(prio);
    }
}
#endif

/*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*
* Description: This function is called by osInit() to initialize the free list of task control blocks.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

static  void  OS_InitTCBList (void)
{
    uint_fast16_t   i;
    OS_TCB         *ptcb;
    OS_TCB        **pptcb;


    OS_MemClr((INT8U *)&OSTCBFreeTbl[0],     sizeof(OSTCBFreeTbl));         //!< Clear all the TCBs
    OS_MemClr((INT8U *)&OSTaskPrioTCBTbl[0], sizeof(OSTaskPrioTCBTbl)); //!< Clear the priority table
    OSTaskList = OS_NULL;                                              //!< TCB lists initializations

    pptcb = &OSTCBFreeList;
    for (i = 0u; i < (OS_MAX_TASKS + OS_N_SYS_TASKS); i++) {    //!< Init. list of free TCBs
        ptcb   = &OSTCBFreeTbl[i];
        *pptcb = ptcb;
        pptcb  = &ptcb->OSTCBNext;
    }
}

/*********************************************************************************************************
*                                           INITIALIZE TCB
*
* Description: This function is internal to OS and is used to get and initialize a Task Control Block when
*              a task is created (see osTaskCreate()).
*
* Arguments  : prio          is the priority of the task being created
*
*              psp           stack pointer when task begin to run. This value return by OSTaskStkInit.
*
*              pstk          point to the LOWEST (valid) memory location of the stack. It's not stack
*                            pointer
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'osTaskCreate()'.
*
*              opt           options as passed to 'osTaskCreate()' or,
*                            0 if called from 'osTaskCreate()'.
*
* Returns    : OS_ERR_NONE              if the call was successful
*              OS_ERR_TASK_NO_MORE_TCB  if there are no more free TCBs to be allocated and thus, the task
*                                       cannot be created.
*
* Note       : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

OS_ERR  OS_TCBInit (INT8U    prio,
                   OS_STK  *psp,
                   OS_STK  *pstk,
                   INT32U   stk_size,
                   INT8U    opt)
{
    OS_TCB     *ptcb;
#if OS_CRITICAL_METHOD == 3u                                //!< Allocate storage for CPU status register
    OS_CPU_SR   cpu_sr = 0u;
#endif


    OS_ENTER_CRITICAL();
    ptcb = OSTCBFreeList;                                   //!< Get a free TCB from the free TCB list
    if (ptcb != OS_NULL) {
        OSTCBFreeList           = ptcb->OSTCBNext;          //!< Update pointer to free TCB list
        OS_EXIT_CRITICAL();
        
        ptcb->OSTCBStkPtr       = psp;                      //!< Load Stack Pointer in TCB

#if OS_EVENT_EN
        ptcb->OSTCBEventPtr     = OS_NULL;                  //!< Task is not pending on an event
#endif

#if OS_FLAG_EN > 0u
        ptcb->OSTCBFlagNode     = OS_NULL;                  //!< Task is not pending on an flag
#endif

        ptcb->OSTCBDly          = 0u;                       //!< Task is not delayed
        ptcb->OSTCBOpt          = opt;                      //!< Store task options
        ptcb->OSTCBStat         = OS_STAT_RDY;              //!< Task is ready to run
        ptcb->OSTCBStatPend     = OS_STAT_PEND_OK;          //!< Clear pend status
        ptcb->OSTCBPrio         = prio;                     //!< Load task priority into TCB

#if OS_MAX_PRIO_LEVELS <= 64u                                   //!< Pre-compute X, Y
        ptcb->OSTCBY            = (INT8U)(prio >> 3u);
        ptcb->OSTCBX            = (INT8U)(prio & 0x07u);
#else                                                       //!< Pre-compute X, Y
        ptcb->OSTCBY            = (INT8U)((INT8U)(prio >> 4u) & 0xFFu);
        ptcb->OSTCBX            = (INT8U) (prio & 0x0Fu);
#endif
                                                            //!< Pre-compute BitX and BitY
        ptcb->OSTCBBitY         = (OS_PRIO)(1u << ptcb->OSTCBY);
        ptcb->OSTCBBitX         = (OS_PRIO)(1u << ptcb->OSTCBX);

#if OS_TASK_PROFILE_EN > 0u                                 //!< Initialize profiling variables
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

        OS_ENTER_CRITICAL();
        OSTaskPrioTCBTbl[prio] = ptcb;
        OS_EXIT_CRITICAL();

#if OS_CPU_HOOKS_EN > 0u
        OSTaskCreateHook(ptcb);
#endif

        OS_ENTER_CRITICAL();
        ptcb->OSTCBNext = OSTaskList;                  //!< Link into active TCB list.
        ptcb->OSTCBPrev = OS_NULL;
        if (OSTaskList != OS_NULL) {
            OSTaskList->OSTCBPrev = ptcb;
        }
        OSTaskList = ptcb;

        OSRdyGrp               |= ptcb->OSTCBBitY;          //!< Add task to ready table
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        
        OSTaskCtr++;                                        //!< Increment the #tasks counter
        OS_EXIT_CRITICAL();
        return OS_ERR_NONE;
    } else {
        OS_EXIT_CRITICAL();
        return OS_ERR_TASK_NO_MORE_TCB;
    }
}

/*********************************************************************************************************
*                           MAKE TASK READY TO RUN BASED ON EVENT OCCURING
*
* Description: This function is called by other OS services and is used to ready a task that was
*              waiting for an event to occur.
*
* Arguments  : pevent      is a pointer to the event control block corresponding to the event.
*
*              msk         is a mask that is used to clear the status byte of the TCB.  For example,
*                          osSemPost() will pass OS_STAT_SEM, OSMboxPost() will pass OS_STAT_MBOX etc.
*
*              pend_stat   is used to indicate the readied task's pending status:
*
*                          OS_STAT_PEND_OK      Task ready due to a event-set, not a timeout or
*                                               an abort.
*                          OS_STAT_PEND_ABORT   Task ready due to an abort(or event was deleted).
*
* Returns    : none
*
* Note       : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

#if (OS_EVENT_EN)
OS_ERR  OS_EventTaskRdy (OS_EVENT  *pevent,
                        INT8U      msk,
                        INT8U      pend_stat)
{
    OS_TCB   *ptcb;
    INT8U     y;
    INT8U     x;
    INT8U     prio;
    OS_PRIO   ptbl;


#if OS_MAX_PRIO_LEVELS <= 64u
    y    = OSUnMapTbl[pevent->OSEventGrp];              //!< Find HPT waiting for message
    ptbl = pevent->OSEventTbl[y];
    x    = OSUnMapTbl[ptbl];
    prio = (INT8U)((y << 3u) + x);                      //!< Find priority of task getting the msg
#else
    if ((pevent->OSEventGrp & 0xFFu) != 0u) {           //!< Find HPT waiting for message
        y = OSUnMapTbl[ pevent->OSEventGrp & 0xFFu];
    } else {
        y = OSUnMapTbl[(OS_PRIO)(pevent->OSEventGrp >> 8u) & 0xFFu] + 8u;
    }
    ptbl = pevent->OSEventTbl[y];
    if ((ptbl & 0xFFu) != 0u) {
        x = OSUnMapTbl[ptbl & 0xFFu];
    } else {
        x = OSUnMapTbl[(OS_PRIO)(ptbl >> 8u) & 0xFFu] + 8u;
    }
    prio = (INT8U)((y << 4u) + x);                      //!< Find priority of task getting the msg
#endif

    ptcb                  =  OSTaskPrioTCBTbl[prio];  //!< Point to this task's OS_TCB
    ptcb->OSTCBDly        =  0u;                        //!< Prevent osTimeTick() from readying task
    ptcb->OSTCBStat      &= (INT8U)~msk;                //!< Clear bit associated with event type
    ptcb->OSTCBStatPend   =  pend_stat;                 //!< Set pend status of post or abort

    OSRdyGrp    |=  ptcb->OSTCBBitY;                    //!< Put task in the ready table
    OSRdyTbl[y] |=  ptcb->OSTCBBitX;

    OS_EventTaskRemove(ptcb, pevent);                   //!< Remove this task from event wait table
    return prio;
}
#endif

/*********************************************************************************************************
*                                  MAKE TASK WAIT FOR EVENT TO OCCUR
*
* Description: This function is called by other OS services to suspend a task because an event has
*              not occurred.
*
* Arguments  : pevent   is a pointer to the event control block for which the task will be waiting for.
*
* Returns    : none
*
* Note       : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

#if (OS_EVENT_EN)
void  OS_EventTaskWait (OS_EVENT *pevent)
{
    INT8U  y;


    OSTCBCur->OSTCBEventPtr = pevent;                   //!< Store ptr to ECB in TCB

    y = OSTCBCur->OSTCBY;

    pevent->OSEventGrp    |= OSTCBCur->OSTCBBitY;       //!< Put task in event's waiting table
    pevent->OSEventTbl[y] |= OSTCBCur->OSTCBBitX;

    OSRdyTbl[y]  &= (OS_PRIO)~OSTCBCur->OSTCBBitX;      //!< Remove task from ready table
    if (OSRdyTbl[y] == 0u) {                            //!< Clear event grp bit if this was only task pending
        OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
    }
}
#endif

/*********************************************************************************************************
*                                  REMOVE TASK FROM EVENT WAIT LIST
*
* Description: Remove a task from an event's wait list.
*
* Arguments  : ptcb     is a pointer to the task to remove.
*
*              pevent   is a pointer to the event control block.
*
* Returns    : none
*
* Note       : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

#if (OS_EVENT_EN)
void  OS_EventTaskRemove (OS_TCB   *ptcb,
                          OS_EVENT *pevent)
{
    INT8U  y;


    y                       =  ptcb->OSTCBY;
    pevent->OSEventTbl[y]  &= (OS_PRIO)~ptcb->OSTCBBitX;    //!< Remove task from wait list
    if (pevent->OSEventTbl[y] == 0u) {
        pevent->OSEventGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBEventPtr     =  OS_NULL;               //!< Unlink OS_EVENT from OS_TCB
}
#endif

/*********************************************************************************************************
*                             INITIALIZE EVENT CONTROL BLOCK'S WAIT LIST
*
* Description: This function is called by other OS services to initialize the event wait list.
*
* Arguments  : pevent    is a pointer to the event control block allocated to the event.
*
* Returns    : none
*
* Note       : This function is INTERNAL to OS and your application should not call it.
*********************************************************************************************************/

#if (OS_EVENT_EN)
void  OS_EventWaitTableInit (OS_EVENT *pevent)
{
    uint_fast8_t  i;


    pevent->OSEventGrp = 0u;                     //!< No task waiting on event
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++) {
        pevent->OSEventTbl[i] = 0u;
    }
}
#endif

/*********************************************************************************************************
*                                             INITIALIZATION
*                           INITIALIZE THE FREE LIST OF EVENT CONTROL BLOCKS
*
* Description: This function is called by osInit() to initialize the free list of event control blocks.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
static  void  OS_InitEventList (void)
{
    uint_fast16_t   i;
    OS_EVENT       *pevent;
    OS_EVENT      **ppevent;


    OS_MemClr((INT8U *)&OSEventFreeTbl[0], sizeof(OSEventFreeTbl));         //!< Clear the event table

    ppevent = &OSEventFreeList;
    for (i = 0u; i < (sizeof(OSEventFreeTbl) / sizeof(OS_EVENT)); i++) {    //!< Init. list of free EVENT control blocks
        pevent = &OSEventFreeTbl[i];
        *ppevent = pevent;
        ppevent  = (OS_EVENT **)&pevent->OSEventPtr;
    }
}
#endif

/*********************************************************************************************************
*                                      CLEAR A SECTION OF MEMORY
*
* Description: This function is called by other OS services to clear a contiguous block of RAM.
*
* Arguments  : pdest    is the start of the RAM to clear (i.e. write 0x00 to)
*
*              size     is the number of bytes to clear.
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to OS and your application should not call it.
*              2) Note that we can only clear up to 0xFFFFFFFF bytes of RAM at once.
*              3) The clear is done one byte at a time since this will work on any processor irrespective
*                 of the alignment of the destination.
*********************************************************************************************************/

void  OS_MemClr (INT8U  *pdest,
                 INT32U  size)
{
    while (size > 0u) {
        *pdest++ = 0u;
        size--;
    }
}

/*********************************************************************************************************
*                                       COPY A BLOCK OF MEMORY
*
* Description: This function is called by other OS services to copy a block of memory from one
*              location to another.
*
* Arguments  : pdest    is a pointer to the 'destination' memory block
*
*              psrc     is a pointer to the 'source'      memory block
*
*              size     is the number of bytes to copy.
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to OS and your application should not call it.  There is
*                 no provision to handle overlapping memory copy.  However, that's not a problem since this
*                 is not a situation that will happen.
*              2) Note that we can only copy up to 0xFFFFFFFF bytes of RAM at once.
*              3) The copy is done one byte at a time since this will work on any processor irrespective
*                 of the alignment of the source and destination.
*********************************************************************************************************/

void  OS_MemCopy (INT8U  *pdest,
                  INT8U  *psrc,
                  INT32U  size)
{
    while (size > 0u) {
        *pdest++ = *psrc++;
        size--;
    }
}

/*********************************************************************************************************
*                                           DUMMY FUNCTION
*
* Description: This function doesn't do anything.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************/

void  OS_Dummy (void)
{
}
