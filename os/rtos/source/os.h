
#ifndef   __RTOS_OS_H__
#define   __RTOS_OS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
*                                           OS VERSION NUMBER
*********************************************************************************************************/

#define  OS_VERSION                 1000u              //!< Version (Vxx.yyy mult. by 1000)

/********************************************************************************************************
*                                        INCLUDE HEADER FILES
*********************************************************************************************************/

#include "..\os_cfg.h"
#include "..\ports\ports.h"

/*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************/
#ifdef  OS_GLOBALS
#   define OS_EXT
#else
#   define OS_EXT  extern
#endif

#define OS_FALSE                     (0u)
#define OS_TRUE                      (1u)

#define OS_INFINITE                 (~0u)

#define OS_NULL               ((void *)0)

#define OS_PRIO_SELF         ((INT8U)~0u)              //!< Indicate SELF priority

#define OS_TCB_RESERVED     ((OS_TCB *)1)

#if OS_STAT_EN > 0u
#   define OS_N_SYS_TASKS            (2u)              //!< Number of system tasks
#else
#   define OS_N_SYS_TASKS            (1u)
#endif

#define OS_TASK_STAT_PRIO       (OS_MAX_PRIO_LEVELS - 2u)           //!< Statistic task priority
#define OS_TASK_IDLE_PRIO       (OS_MAX_PRIO_LEVELS - 1u)           //!< IDLE task priority

#if OS_MAX_PRIO_LEVELS <= 64u
#   define OS_EVENT_TBL_SIZE    ((OS_MAX_PRIO_LEVELS + 7u) / 8u)    //!< Size of event table
#   define OS_RDY_TBL_SIZE      ((OS_MAX_PRIO_LEVELS + 7u) / 8u)    //!< Size of ready table
#else
#   define OS_EVENT_TBL_SIZE    ((OS_MAX_PRIO_LEVELS + 15u) / 16u)  //!< Size of event table
#   define OS_RDY_TBL_SIZE      ((OS_MAX_PRIO_LEVELS + 15u) / 16u)  //!< Size of ready table
#endif

#define OS_EVENT_EN             ((OS_SEM_EN > 0u) || (OS_MUTEX_EN > 0u))

/*********************************************************************************************************
*                             TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************/
#define OS_STAT_RDY             (0x00u)   //!< Ready to run
#define OS_STAT_SEM             (0x01u)   //!< Pending on semaphore
#define OS_STAT_MUTEX           (0x02u)   //!< Pending on mutual exclusion semaphore
#define OS_STAT_FLAG            (0x04u)   //!< Pending on flag

#define OS_STAT_PEND_ANY        (OS_STAT_SEM | OS_STAT_MUTEX | OS_STAT_FLAG)

/*********************************************************************************************************
*                          TASK PEND STATUS (Status codes for OSTCBStatPend)
*********************************************************************************************************/
#define OS_STAT_PEND_OK         (0u)      //!< Not pending, or pending complete
#define OS_STAT_PEND_ONGOING    (1u)      //!< Pending ongoing
#define OS_STAT_PEND_TO         (2u)      //!< Pending timed out
#define OS_STAT_PEND_ABORT      (3u)      //!< Pending aborted

/*********************************************************************************************************
*                                           OS_EVENT types
*********************************************************************************************************/
#define OS_OBJ_TYPE_UNUSED      (0u)
#define OS_OBJ_TYPE_SEM         (1u)
#define OS_OBJ_TYPE_MUTEX       (2u)
#define OS_OBJ_TYPE_FLAG        (3u)
#define OS_OBJ_TYPE_TCB         (4u)

/*********************************************************************************************************
*      Possible values for 'opt' argument of osSemDelete(), osFlagDelete() and osMutexDelete()
*********************************************************************************************************/
#define OS_DEL_NO_PEND          (0u)
#define OS_DEL_ALWAYS           (1u)

/*********************************************************************************************************
*                                        OS???Pend() OPTIONS
*
* These #defines are used to establish the options for OS???PendAbort().
*********************************************************************************************************/
#define OS_PEND_OPT_NONE        (0u)       //!< NO option selected
#define OS_PEND_OPT_BROADCAST   (1u)       //!< Broadcast action to ALL tasks waiting

/*********************************************************************************************************
*                                TASK OPTIONS (see osTaskCreate())
*********************************************************************************************************/
#define OS_TASK_OPT_NONE        (0x00u)    //!< NO option selected
#define OS_TASK_OPT_STK_CHK     (0x01u)    //!< Enable stack checking for the task
#define OS_TASK_OPT_STK_CLR     (0x02u)    //!< Clear the stack when the task is create
#define OS_TASK_OPT_SAVE_FP     (0x04u)    //!< Save the contents of any floating-point registers

/*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************/
#define OS_ERR_NONE                     0u

#define OS_ERR_EVENT_TYPE               1u
#define OS_ERR_PEND_ISR                 2u
#define OS_ERR_PDATA_NULL               3u
#define OS_ERR_PEVENT_NULL              4u
#define OS_ERR_POST_ISR                 5u
#define OS_ERR_QUERY_ISR                6u
#define OS_ERR_INVALID_OPT              7u
#define OS_ERR_NOT_RUNNING              8u

#define OS_ERR_TIMEOUT                 10u
#define OS_ERR_PEND_LOCKED             13u
#define OS_ERR_PEND_ABORT              14u
#define OS_ERR_DEL_ISR                 15u
#define OS_ERR_CREATE_ISR              16u
#define OS_ERR_ILLEGAL_CREATE_RUN_TIME 19u

#define OS_ERR_PRIO_EXIST              40u
#define OS_ERR_PRIO                    41u
#define OS_ERR_PRIO_INVALID            42u

#define OS_ERR_TASK_CREATE_ISR         60u
#define OS_ERR_TASK_DEL                61u
#define OS_ERR_TASK_DEL_IDLE           62u
#define OS_ERR_TASK_DEL_REQ            63u
#define OS_ERR_TASK_DEL_ISR            64u
#define OS_ERR_TASK_NO_MORE_TCB        66u
#define OS_ERR_TASK_NOT_EXIST          67u
#define OS_ERR_TASK_NOT_SUSPENDED      68u
#define OS_ERR_TASK_OPT                69u
#define OS_ERR_TASK_RESUME_PRIO        70u
#define OS_ERR_TASK_SUSPEND_IDLE       71u
#define OS_ERR_TASK_SUSPEND_PRIO       72u
#define OS_ERR_TASK_WAITING            73u

#define OS_ERR_EVENT_DEPLETED          90u
#define OS_ERR_SEM_OVF                 91u
#define OS_ERR_PCP_LOWER               92u
#define OS_ERR_NOT_MUTEX_OWNER         93u

#define OS_ERR_FLAG_PFLAG_NULL        100u
#define OS_ERR_FLAG_DEPLETED          101u

    
/*********************************************************************************************************
*                                           ELEMENTARY TYPE
*********************************************************************************************************/
typedef INT8U   OS_ERR;

#if OS_MAX_PRIO_LEVELS <= 64u
typedef INT8U   OS_PRIO;
#else
typedef INT16U  OS_PRIO;
#endif

typedef struct os_bitmap {
    OS_PRIO     Group;
    OS_PRIO     Matrix;
} OS_BITMAP;

typedef void *  OS_HANDLE;

#define OS_OBJ_STRUCT           \
    INT16U      OSObjType;      \
    INT16U      OSObjKey;
    
typedef struct os_obj {
    OS_OBJ_STRUCT
} OS_OBJ;

    
/*********************************************************************************************************
*                                         EVENT CONTROL BLOCK
*********************************************************************************************************/
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
typedef struct os_event {
    OS_OBJ_STRUCT
    void           *OSEventPtr;                     //!< Pointer to Mutex owner's TCB
    INT16U          OSEventCnt;                     //!< Semaphore Count or Mutex owner priority
    OS_PRIO         OSEventGrp;                     //!< Group corresponding to tasks waiting for event to occur
    OS_PRIO         OSEventTbl[OS_EVENT_TBL_SIZE];  //!< List of tasks waiting for event to occur
} OS_EVENT;
#endif

/*********************************************************************************************************
*                                      EVENT FLAGS CONTROL BLOCK
*********************************************************************************************************/
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
typedef struct os_flag {
    OS_OBJ_STRUCT
    void           *OSFlagWaitList;                 //!< Pointer to first NODE of task waiting on event flag
    INT16U          OSFlagFlags;                    //!< Flag options
} OS_FLAG;

typedef struct os_flag_node {                       //!< Event Flag Wait List Node
    void           *OSFlagNodeNext;                 //!< Pointer to next     NODE in wait list
    void           *OSFlagNodePrev;                 //!< Pointer to previous NODE in wait list
    void           *OSFlagNodeTCB;                  //!< Pointer to TCB of waiting task
    void           *OSFlagNodeFlag;                 //!< Pointer to Event Flag
} OS_FLAG_NODE;
#endif

/*********************************************************************************************************
*                                         TASK CONTROL BLOCK
*********************************************************************************************************/
typedef struct os_tcb {
    OS_OBJ_STRUCT
    OS_STK         *OSTCBStkPtr;                    //!< Pointer to current top of stack

    struct os_tcb  *OSTCBNext;                      //!< Pointer to next     TCB in the TCB list
    struct os_tcb  *OSTCBPrev;                      //!< Pointer to previous TCB in the TCB list

#if OS_EVENT_EN
    OS_EVENT       *OSTCBEventPtr;                  //!< Pointer to event control block
#endif

#if OS_FLAG_EN > 0u
    OS_FLAG_NODE   *OSTCBFlagNode;                  //!< Pointer to event flag node
#endif

    INT32U          OSTCBDly;                       //!< Nbr ticks to delay task or, timeout waiting for event
    INT8U           OSTCBPrio;                      //!< Task priority (0 == highest)
    INT8U           OSTCBOpt;                       //!< Task options as passed by osTaskCreate()
    INT8U           OSTCBStat;                      //!< Task      status
    INT8U           OSTCBStatPend;                  //!< Task PEND status

    INT8U           OSTCBX;                         //!< Bit position in group  corresponding to task priority
    INT8U           OSTCBY;                         //!< Index into ready table corresponding to task priority
    OS_PRIO         OSTCBBitX;                      //!< Bit mask to access bit position in ready table
    OS_PRIO         OSTCBBitY;                      //!< Bit mask to access bit position in ready group

#if OS_TASK_PROFILE_EN > 0u
    OS_STK         *OSTCBStkBase;                   //!< Pointer to the beginning of the task stack
    INT32U          OSTCBStkSize;                   //!< Size of task stack (in number of stack elements)
    INT32U          OSTCBStkUsed;                   //!< Number of bytes used from the stack
    INT32U          OSTCBCtxSwCtr;                  //!< Number of times the task was switched in
    INT32U          OSTCBCyclesTot;                 //!< Total number of ticks the task has been running
    INT32U          OSTCBCyclesStart;               //!< Snapshot of tick at start of task
#endif
} OS_TCB;

/*********************************************************************************************************
*                                               FLAG DATA
*********************************************************************************************************/
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
typedef struct os_flag_data {
    BOOLEAN         OSFlagAutoReset;
    BOOLEAN         OSFlagStatus;
} OS_FLAG_DATA;
#endif

/*********************************************************************************************************
*                                   MUTUAL EXCLUSION SEMAPHORE DATA
*********************************************************************************************************/
#if OS_MUTEX_EN > 0u
typedef struct os_mutex_data {
    OS_PRIO         OSEventGrp;
    OS_PRIO         OSEventTbl[OS_EVENT_TBL_SIZE];
    BOOLEAN         OSValue;                        //!< Mutex value (OS_FALSE = used, OS_TRUE = available)
    INT8U           OSOwnerPrio;                    //!< Mutex owner's task priority or 0xFF if no owner
} OS_MUTEX_DATA;
#endif

/*********************************************************************************************************
*                                           SEMAPHORE DATA
*********************************************************************************************************/
#if OS_SEM_EN > 0u
typedef struct os_sem_data {
    OS_PRIO         OSEventGrp;
    OS_PRIO         OSEventTbl[OS_EVENT_TBL_SIZE];
    INT16U          OSCnt;                          //!< Semaphore count
} OS_SEM_DATA;
#endif

/*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************/
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
OS_EXT  OS_EVENT   *OSEventFreeList;                //!< Pointer to list of free EVENT control blocks
OS_EXT  OS_EVENT    OSEventFreeTbl[OS_MAX_EVENTS];      //!< Table of EVENT control blocks
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
OS_EXT  OS_FLAG    *OSFlagFreeList;                 //!< Pointer to list of free event flag control blocks
OS_EXT  OS_FLAG     OSFlagFreeTbl[OS_MAX_FLAGS];        //!< Table of event flag control blocks
#endif

OS_EXT  OS_TCB     *OSTCBFreeList;                                  //!< List of free TCBs
OS_EXT  OS_TCB      OSTCBFreeTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];    //!< Table of free TCBs

OS_EXT  OS_TCB     *OSTaskList;                     //!< Doubly linked list of active task's TCB
OS_EXT  INT8U       OSTaskCtr;                      //!< Number of active tasks

OS_EXT  OS_PRIO     OSRdyGrp;                       //!< Ready list group
OS_EXT  OS_PRIO     OSRdyTbl[OS_RDY_TBL_SIZE];      //!< Table of tasks which are ready to run
OS_EXT  OS_TCB     *OSTaskPrioTCBTbl[OS_MAX_PRIO_LEVELS];   //!< Table of pointers to TCB of active task

OS_EXT  OS_TCB     *OSTCBCur;                       //!< Pointer to currently running TCB
OS_EXT  OS_TCB     *OSTCBHighRdy;                   //!< Pointer to highest priority TCB Ready-to-Run

OS_EXT  INT8U       OSIntNesting;                   //!< Interrupt nesting level
OS_EXT  INT8U       OSLockNesting;                  //!< Multitasking lock nesting level

OS_EXT  BOOLEAN     OSRunning;                      //!< Flag indicating that kernel is running

OS_EXT  volatile INT32U OSIdleCtr;                  //!< Idle counter
    
#if OS_STAT_EN > 0u
OS_EXT  INT32U      OSCtxSwCtr;                     //!< Counter of number of context switches
OS_EXT  INT8U       OSCPUUsage;                     //!< Percentage of CPU used
OS_EXT  INT32U      OSIdleCtrMax;                   //!< Max. value that idle ctr can take in 1 sec.
OS_EXT  BOOLEAN     OSStatRunning;                  //!< Flag indicating that the statistic task is running
OS_EXT  OS_STK      OSTaskStatStk[OS_TASK_STAT_STK_SIZE];       //!< Statistics task stack
#endif

OS_EXT  OS_STK      OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];       //!< Idle task stack

/*********************************************************************************************************
*                                       EVENT FLAGS MANAGEMENT
*********************************************************************************************************/
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)

OS_ERR      osFlagCreate           (OS_HANDLE      *pFlagHandle,
                                    BOOLEAN         init,
                                    BOOLEAN         manual);

#if OS_FLAG_DEL_EN > 0u
OS_ERR      osFlagDelete           (OS_HANDLE       hFlag,
                                    INT8U           opt);
#endif

OS_ERR      osFlagPend             (OS_HANDLE       hFlag,
                                    INT32U          timeout);

OS_ERR      osFlagSet              (OS_HANDLE       hFlag);

OS_ERR      osFlagReset            (OS_HANDLE       hFlag);

#if OS_FLAG_QUERY_EN > 0u
OS_ERR      osFlagQuery            (OS_HANDLE       hFlag,
                                    OS_FLAG_DATA   *p_flag_data);
#endif

#endif

/*********************************************************************************************************
*                                MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
*********************************************************************************************************/
#if OS_MUTEX_EN > 0u

OS_ERR      osMutexCreate          (OS_HANDLE      *pMutexHandle);

#if OS_MUTEX_DEL_EN > 0u
OS_ERR      osMutexDelete          (OS_HANDLE       hMutex,
                                    INT8U           opt);
#endif

OS_ERR      osMutexPend            (OS_HANDLE       hMutex,
                                    INT32U          timeout);

OS_ERR      osMutexPost            (OS_HANDLE       hMutex);

#if OS_MUTEX_QUERY_EN > 0u
OS_ERR      osMutexQuery           (OS_HANDLE       hMutex,
                                    OS_MUTEX_DATA  *p_mutex_data);
#endif

#endif

/*********************************************************************************************************
*                                        SEMAPHORE MANAGEMENT
*********************************************************************************************************/
#if OS_SEM_EN > 0u

OS_ERR      osSemCreate            (OS_HANDLE      *pSemaphoreHandle,
                                    INT16U          cnt);

#if OS_SEM_DEL_EN > 0u
OS_ERR      osSemDelete            (OS_HANDLE       hSemaphore,
                                    INT8U           opt);
#endif

OS_ERR      osSemPend              (OS_HANDLE       hSemaphore,
                                    INT32U          timeout);

#if OS_SEM_PEND_ABORT_EN > 0u
OS_ERR      osSemPendAbort         (OS_HANDLE       hSemaphore,
                                    INT8U           opt);
#endif

OS_ERR      osSemPost              (OS_HANDLE       hSemaphore,
                                    INT16U          cnt);

#if OS_SEM_SET_EN > 0u
OS_ERR      osSemSet               (OS_HANDLE       hSemaphore,
                                    INT16U          cnt);
#endif

#if OS_SEM_QUERY_EN > 0u
OS_ERR      osSemQuery             (OS_HANDLE       hSemaphore,
                                    OS_SEM_DATA    *p_sem_data);
#endif

#endif

/*********************************************************************************************************
*                                           TASK MANAGEMENT
*********************************************************************************************************/
OS_ERR      osTaskCreate           (void          (*task)(void *parg),
                                    void           *parg,
                                    INT8U           prio,
                                    OS_STK         *pstk,
                                    INT32U          stk_size,
                                    INT8U           opt);

#if OS_TASK_CHANGE_PRIO_EN > 0u
OS_ERR      osTaskChangePrio       (INT8U           oldprio,
                                    INT8U           newprio);
#endif

#if OS_TASK_QUERY_EN > 0u
OS_ERR      osTaskQuery            (INT8U           prio,
                                    OS_TCB         *p_task_data);
#endif

/*********************************************************************************************************
*                                           TIME MANAGEMENT
*********************************************************************************************************/
void        osTimeDelay            (INT32U          ticks);

void        osTimeTick             (void);

/*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************/
void        osInit                 (void);

void        osIntEnter             (void);
void        osIntExit              (void);

#if OS_SCHED_LOCK_EN > 0u
void        osSchedLock            (void);
void        osSchedUnlock          (void);
#endif

void        osStart                (void);

#if OS_STAT_EN > 0u
void        osStatInit             (void);
#endif

INT16U      osVersion              (void);

/*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                              (Target Specific interface and hook Functions)
*********************************************************************************************************/
void        OSStartHighRdy         (void);
void        OSIntCtxSw             (void);
void        OSCtxSw                (void);

OS_STK     *OSTaskStkInit          (void            *task,
                                    void            *parg,
                                    OS_STK          *ptos,
                                    void            *exit);

void        OSInitHookBegin        (void);
void        OSInitHookEnd          (void);

void        OSTaskCreateHook       (OS_TCB          *ptcb);
void        OSTaskReturnHook       (OS_TCB          *ptcb);

#if OS_TASK_SW_HOOK_EN > 0u
void        OSTaskSwHook           (void);
#endif

void        OSTCBInitHook          (OS_TCB          *ptcb);

void        OSTaskIdleHook         (void);

void        OSTaskStatHook         (void);

#if OS_TIME_TICK_HOOK_EN > 0u
void        OSTimeTickHook         (void);
#endif

#if OS_DEBUG_EN > 0u
void        OSDebugInit            (void);
#endif

/*********************************************************************************************************
*                                      INTERNAL FUNCTION PROTOTYPES
*                            (Your application MUST NOT call these functions)
*********************************************************************************************************/
void        OS_Sched               (void);

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
OS_ERR      OS_TaskStkChk          (INT8U           prio);
#endif

#if (OS_EVENT_EN)
OS_ERR      OS_EventTaskRdy        (OS_EVENT       *pevent,
                                    INT8U           msk,
                                    INT8U           pend_stat);

void        OS_EventTaskWait       (OS_EVENT       *pevent);

void        OS_EventTaskRemove     (OS_TCB         *ptcb,
                                    OS_EVENT       *pevent);

void        OS_EventWaitTableInit  (OS_EVENT       *pevent);
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
void        OS_FlagListInit        (void);

void        OS_FlagTaskRemove      (OS_FLAG_NODE   *pnode);
#endif

void        OS_MemClr              (INT8U          *pdest,
                                    INT32U          size);

void        OS_MemCopy             (INT8U          *pdest,
                                    INT8U          *psrc,
                                    INT32U          size);

void        OS_Dummy               (void);

OS_ERR      OS_TCBInit             (INT8U           prio,
                                    OS_STK         *psp,
                                    OS_STK         *pstk,
                                    INT32U          stk_size,
                                    INT8U           opt);

/*********************************************************************************************************
*                                   LOOK FOR MISSING #define CONSTANTS
*
* This section is used to generate ERROR messages at compile time if certain #define constants are
* MISSING in OS_CFG.H.  This allows you to quickly determine the source of the error.
*
* You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
* compile time error.
*********************************************************************************************************/

/*********************************************************************************************************
*                                             EVENT FLAGS
*********************************************************************************************************/
#ifndef OS_FLAG_EN
#   error  "OS_CFG.H, Missing OS_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
#   ifndef OS_MAX_FLAGS
#       error  "OS_CFG.H, Missing OS_MAX_FLAGS: Max. number of Event Flag Groups in your application"
#   else
#       if     OS_MAX_FLAGS > 65500u
#           error  "OS_CFG.H, OS_MAX_FLAGS must be <= 65500"
#       endif
#   endif
#   ifndef OS_FLAG_DEL_EN
#       error  "OS_CFG.H, Missing OS_FLAG_DEL_EN: Include code for osFlagDelete()"
#   endif
#   ifndef OS_FLAG_QUERY_EN
#       error  "OS_CFG.H, Missing OS_FLAG_QUERY_EN: Include code for osFlagQuery()"
#   endif
#endif

/*********************************************************************************************************
*                                     MUTUAL EXCLUSION SEMAPHORES
*********************************************************************************************************/
#ifndef OS_MUTEX_EN
#   error  "OS_CFG.H, Missing OS_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#else
#   ifndef OS_MUTEX_DEL_EN
#       error  "OS_CFG.H, Missing OS_MUTEX_DEL_EN: Include code for osMutexDelete()"
#   endif
#   ifndef OS_MUTEX_QUERY_EN
#       error  "OS_CFG.H, Missing OS_MUTEX_QUERY_EN: Include code for osMutexQuery()"
#   endif
#endif

/*********************************************************************************************************
*                                             SEMAPHORES
*********************************************************************************************************/
#ifndef OS_SEM_EN
#   error  "OS_CFG.H, Missing OS_SEM_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#else
#   ifndef OS_SEM_DEL_EN
#       error  "OS_CFG.H, Missing OS_SEM_DEL_EN: Include code for osSemDelete()"
#   endif
#   ifndef OS_SEM_PEND_ABORT_EN
#       error  "OS_CFG.H, Missing OS_SEM_PEND_ABORT_EN: Include code for osSemPendAbort()"
#   endif
#   ifndef OS_SEM_QUERY_EN
#       error  "OS_CFG.H, Missing OS_SEM_QUERY_EN: Include code for osSemQuery()"
#   endif
#   ifndef OS_SEM_SET_EN
#       error  "OS_CFG.H, Missing OS_SEM_SET_EN: Include code for osSemSet()"
#   endif
#endif

/*********************************************************************************************************
*                                           TASK MANAGEMENT
*********************************************************************************************************/
#ifndef OS_MAX_TASKS
#   error  "OS_CFG.H, Missing OS_MAX_TASKS: Max. number of tasks in your application"
#else
#   if     OS_MAX_TASKS < 2u
#       error  "OS_CFG.H,         OS_MAX_TASKS must be >= 2"
#   endif
#   if     OS_MAX_TASKS >  (OS_MAX_PRIO_LEVELS - OS_N_SYS_TASKS)
#       error  "OS_CFG.H,         OS_MAX_TASKS must be <= OS_MAX_PRIO_LEVELS - OS_N_SYS_TASKS"
#   endif
#endif

#if OS_MAX_PRIO_LEVELS <  2u
#   error  "OS_CFG.H,         OS_MAX_PRIO_LEVELS must be >= 2"
#endif

#if OS_MAX_PRIO_LEVELS >  255u
#   error  "OS_CFG.H,         OS_MAX_PRIO_LEVELS must be <= 255"
#endif

#ifndef OS_TASK_IDLE_STK_SIZE
#   error  "OS_CFG.H, Missing OS_TASK_IDLE_STK_SIZE: Idle task stack size"
#endif

#ifndef OS_STAT_EN
#   error  "OS_CFG.H, Missing OS_STAT_EN: Enable (1) or Disable(0) the statistics task"
#endif

#ifndef OS_TASK_STAT_STK_SIZE
#   error  "OS_CFG.H, Missing OS_TASK_STAT_STK_SIZE: Statistics task stack size"
#endif

#ifndef OS_STAT_TASK_STK_CHK_EN
#   error  "OS_CFG.H, Missing OS_STAT_TASK_STK_CHK_EN: Check task stacks from statistics task"
#endif

#ifndef OS_TASK_CHANGE_PRIO_EN
#   error  "OS_CFG.H, Missing OS_TASK_CHANGE_PRIO_EN: Include code for osTaskChangePrio()"
#endif

#ifndef OS_TASK_QUERY_EN
#   error  "OS_CFG.H, Missing OS_TASK_QUERY_EN: Include code for osTaskQuery()"
#endif

/*********************************************************************************************************
*                                           TIME MANAGEMENT
*********************************************************************************************************/
#ifndef OS_TICKS_PER_SEC
#   error  "OS_CFG.H, Missing OS_TICKS_PER_SEC: Sets the number of ticks in one second"
#else
#   if  OS_TICKS_PER_SEC == 0
#       error   "OS_CFG.H, OS_TICKS_PER_SEC must be > 0"
#   endif
#endif

/*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************/
#ifndef OS_ARG_CHK_EN
#   error  "OS_CFG.H, Missing OS_ARG_CHK_EN: Enable (1) or Disable (0) argument checking"
#endif

#ifndef OS_CPU_HOOKS_EN
#   error  "OS_CFG.H, Missing OS_CPU_HOOKS_EN: OS hooks are found in the processor port files when 1"
#endif

#ifndef OS_DEBUG_EN
#   error  "OS_CFG.H, Missing OS_DEBUG_EN: Allows you to include variables for debugging or not"
#endif

#ifndef OS_MAX_PRIO_LEVELS
#   error  "OS_CFG.H, Missing OS_MAX_PRIO_LEVELS: Defines the lowest priority that can be assigned"
#endif

#ifndef OS_MAX_EVENTS
#   error  "OS_CFG.H, Missing OS_MAX_EVENTS: Max. number of event control blocks in your application"
#else
#   if     OS_MAX_EVENTS > 65500u
#       error  "OS_CFG.H, OS_MAX_EVENTS must be <= 65500"
#   endif
#endif

#ifndef OS_SCHED_LOCK_EN
#   error  "OS_CFG.H, Missing OS_SCHED_LOCK_EN: Include code for osSchedLock() and osSchedUnlock()"
#endif

#ifndef OS_TASK_SW_HOOK_EN
#   error  "OS_CFG.H, Missing OS_TASK_SW_HOOK_EN: Allows you to include the code for OSTaskSwHook() or not"
#endif

#ifndef OS_TIME_TICK_HOOK_EN
#   error  "OS_CFG.H, Missing OS_TIME_TICK_HOOK_EN: Allows you to include the code for OSTimeTickHook() or not"
#endif

/*********************************************************************************************************
*                                         SAFETY CRITICAL USE
*********************************************************************************************************/
#ifdef SAFETY_CRITICAL_RELEASE

#if    OS_ARG_CHK_EN < 1u
#   error "OS_CFG.H, OS_ARG_CHK_EN must be enabled for safety-critical release code"
#endif

#if    OS_DEBUG_EN > 0u
#   error "OS_CFG.H, OS_DEBUG_EN must be disabled for safety-critical release code"
#endif

#ifdef OS_SCHED_LOCK_EN
#   error "OS_CFG.H, OS_SCHED_LOCK_EN must be disabled for safety-critical release code"
#endif

#if    OS_STAT_EN > 0u
#   error "OS_CFG.H, OS_STAT_EN must be disabled for safety-critical release code"
#endif

#if    OS_FLAG_EN > 0u
#   if    OS_FLAG_DEL_EN > 0
#       error "OS_CFG.H, OS_FLAG_DEL_EN must be disabled for safety-critical release code"
#   endif
#endif

#if    OS_MUTEX_EN > 0u
#   if    OS_MUTEX_DEL_EN > 0u
#       error "OS_CFG.H, OS_MUTEX_DEL_EN must be disabled for safety-critical release code"
#   endif
#endif

#if    OS_SEM_EN > 0u
#   if    OS_SEM_DEL_EN > 0u
#       error "OS_CFG.H, OS_SEM_DEL_EN must be disabled for safety-critical release code"
#   endif
#endif

#if    OS_CRITICAL_METHOD != 3u
#   error "OS_CPU.H, OS_CRITICAL_METHOD must be type 3 for safety-critical release code"
#endif

#endif  //!< SAFETY_CRITICAL_RELEASE

#ifdef __cplusplus
}
#endif

#endif
