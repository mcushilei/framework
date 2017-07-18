
//! \note do not move this pre-processor statement to other places
#ifndef   __OS_H__
#define   __OS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *! OS VERSION NUMBER
 */
#define  OS_VERSION                 1000u       //!< Version (Vxx.yyy mult. by 1000)

/*!
 *! INCLUDE HEADER FILES
 */
#include "..\os_cfg.h"
#include "..\ports\ports.h"

/*!
 *! MISCELLANEOUS
 */
#ifdef  __OS_CORE_C__
#   define OS_EXT
#else
#   define OS_EXT  extern
#endif

#define OS_INFINITE                 (~0u)

#if OS_STAT_EN > 0u
#   define OS_N_SYS_TASKS           (2u)        //!< Number of system tasks
#else
#   define OS_N_SYS_TASKS           (1u)
#endif

#define OS_TASK_LOWEST_PRIO         (OS_MAX_PRIO_LEVELS - 1u)
#define OS_TASK_IDLE_PRIO           (OS_MAX_PRIO_LEVELS - 1u)           //!< IDLE task priority
#define OS_TASK_STAT_PRIO           (OS_MAX_PRIO_LEVELS - 2u)           //!< Statistic task priority

#if OS_MAX_PRIO_LEVELS <= 64u
#   define OS_BITMAP_TBL_SIZE       ((OS_MAX_PRIO_LEVELS + 7u) / 8u)    //!< Size of ready table
#else
#   define OS_BITMAP_TBL_SIZE       ((OS_MAX_PRIO_LEVELS + 15u) / 16u)  //!< Size of ready table
#endif

#define OS_EVENT_EN                 (OS_MUTEX_EN | OS_FLAG_EN |OS_SEMP_EN)
    
#define OS_CONTAINER_OF(__ptr, __type, __member) (              \
        (__type*)( (char*)(__ptr) - offsetof(__type, __member) ))

/*!
 *! TASK PEND STATUS (Status codes for OSTCBStatPend)
 */
#define OS_STAT_PEND_OK             (0u)      //!< Not pending, or pending complete
#define OS_STAT_PEND_TO             (1u)      //!< Pending timed out
#define OS_STAT_PEND_ABORT          (2u)      //!< Pending aborted

/*!
 *! OBJECT TYPES
 */
enum {
    OS_OBJ_TYPE_UNUSED = 0,
    OS_OBJ_TYPE_SEMP,
    OS_OBJ_TYPE_MUTEX,
    OS_OBJ_TYPE_FLAG,
    OS_OBJ_TYPE_TCB,
};
#define OS_OBJ_TYPE_MSK             (0x0Fu << 0)
#define OS_OBJ_TYPE_SET(__OT)       ((UINT16)( ((__OT) << 0) & OS_OBJ_TYPE_MSK ))
#define OS_OBJ_TYPE_GET(__OT)       ((UINT16)( ((__OT) & OS_OBJ_TYPE_MSK) >> 0 ))

#define OS_OBJ_PRIO_TYPE_LIST       (0u)
#define OS_OBJ_PRIO_TYPE_PRIO_LIST  (1u)
#define OS_OBJ_PRIO_TYPE_MSK        (0x03u << 8)
#define OS_OBJ_PRIO_TYPE_SET(__OT)  ((UINT16)( ((__OT) << 8) & OS_OBJ_TYPE_MSK ))
#define OS_OBJ_PRIO_TYPE_GET(__OT)  ((UINT16)( ((__OT) & OS_OBJ_TYPE_MSK) >> 8 ))

#define OS_OBJ_WAITABLE             (1u << 15)

/*!
 *! OS???Delete() OPTIONS
 */
#define OS_DEL_NO_PEND              (0u)
#define OS_DEL_ALWAYS               (1u)

/*!
 *! OS???Pend() OPTIONS
 *!
 *! These #defines are used to establish the options for OS???PendAbort().
 */
#define OS_PEND_OPT_NONE            (0u)       //!< NO option selected
#define OS_PEND_OPT_BROADCAST       (1u)       //!< Broadcast action to ALL tasks waiting

/*!
 *! TASK OPTIONS (see osTaskCreate())
 */
#define OS_TASK_OPT_NONE            (0x00u)    //!< NO option selected
#define OS_TASK_OPT_STK_CHK         (0x01u)    //!< Enable stack checking for the task
#define OS_TASK_OPT_STK_CLR         (0x02u)    //!< Clear the stack when the task is create
#define OS_TASK_OPT_SAVE_FP         (0x04u)    //!< Save the contents of any floating-point registers

/*!
 *! ERROR CODES
 */
enum {
    OS_ERR_NONE                     = 0u,

    OS_ERR_EVENT_TYPE               = 1u,
    OS_ERR_PDATA_NULL               = 2u,
    OS_ERR_INVALID_HANDLE           = 3u,
    OS_ERR_INVALID_OPT              = 4u,
    OS_ERR_DEL_ISR                  = 5u,
    OS_ERR_CREATE_ISR               = 6u,

    OS_ERR_TIMEOUT                  = 30u,
    OS_ERR_PEND_ISR                 = 31u,
    OS_ERR_PEND_LOCKED              = 32u,
    OS_ERR_PEND_ABORT               = 33u,
    OS_ERR_POST_ISR                 = 34u,
    OS_ERR_TASK_WAITING             = 35u,

    OS_ERR_TASK_DEPLETED            = 60u,
    OS_ERR_TASK_OPT                 = 61u,
    OS_ERR_TASK_EXIST               = 62u,
    OS_ERR_TASK_NOT_EXIST           = 63u,
    OS_ERR_INVALID_PRIO             = 64u,

    OS_ERR_FLAG_DEPLETED            = 80u,

    OS_ERR_EVENT_DEPLETED           = 90u,
    OS_ERR_SEM_OVF                  = 91u,
    OS_ERR_NOT_MUTEX_OWNER          = 92u,
    OS_ERR_HAS_OWN_MUTEX            = 93u,
};
    
/*!
 *! ELEMENTARY TYPE
 */
typedef UINT8   OS_ERR;

#if OS_MAX_PRIO_LEVELS <= 64u
typedef UINT8   OS_PRIO;
#else
typedef UINT16  OS_PRIO;
#endif

typedef void    OS_TASK(void *);

typedef void*   OS_HANDLE;

typedef struct list_node        OS_LIST_NODE;
typedef struct os_mem_pool      OS_MEM_POOL;
typedef struct os_waitable_obj  OS_WAITBALE_OBJ;
typedef struct os_tcb           OS_TCB;
typedef struct os_flag          OS_FLAG;
typedef struct os_mutex         OS_MUTEX;
typedef struct os_semp          OS_SEMP;
typedef struct os_wait_node     OS_WAIT_NODE;

typedef struct {
    UINT16          OSObjType;
} OS_OBJ_HEAD;

//! list type
struct list_node {
    OS_LIST_NODE   *Prev;
    OS_LIST_NODE   *Next;
};

//! memory pool.
struct os_mem_pool {
    OS_LIST_NODE    OSMemList;
};

struct os_wait_node {                           //!< Event Wait List Node.
    OS_LIST_NODE        OSWaitNodeList;         //!< Pointer to previous NODE in wait list.
    OS_TCB             *OSWaitNodeTCB;          //!< Pointer to TCB of waiting task.
    OS_WAITBALE_OBJ    *OSWaitNodeECB;          //!< Pointer to ECB wait for.
    UINT8               OSWaitNodeRes;          //!< Event wait resault.
};
    
struct os_waitable_obj {                        //!< Waitable object head.
    OS_OBJ_HEAD;
    UINT16          OSWaitObjCnt;               //!< counter.
    OS_LIST_NODE    OSWaitObjWaitList;          //!< Pointer to first NODE of task waiting on this object.
};
    
/*!
 *! SEMAPHORE CONTROL BLOCK
 */
#if (OS_SEMP_EN) && (OS_MAX_SEMAPHORES > 0u)
struct os_semp {
    OS_OBJ_HEAD;
    UINT16          OSSempCnt;                  //!< Semaphore count.
    OS_LIST_NODE    OSSempWaitList;             //!< Pointer to first NODE of task waiting on semaphore
};
#endif

/*!
 *! MUTEX SEMAPHORE CONTROL BLOCK
 */
#if (OS_MUTEX_EN) && (OS_MAX_MUTEXES > 0u)
struct os_mutex {
    OS_OBJ_HEAD;
    UINT8           OSMutexCeilingPrio;         //!< Mutex's ceiling prio.
    UINT8           OSMutexOwnerPrio;           //!< Mutex owner's prio.
    OS_LIST_NODE    OSMutexWaitList;            //!< Pointer to first NODE of task waiting on mutex
    OS_TCB         *OSMutexOwnerTCB;            //!< Pointer to mutex owner's TCB
};
#endif

/*!
 *! EVENT FLAGS CONTROL BLOCK
 */
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
struct os_flag {
    OS_OBJ_HEAD;
    UINT16          OSFlagFlags;                //!< Flag options
    OS_LIST_NODE    OSFlagWaitList;             //!< Pointer to first NODE of task waiting on flag
};
#endif

/*!
 *! TASK CONTROL BLOCK
 */
struct os_tcb {
    OS_OBJ_HEAD;
    
    UINT8           OSTCBOpt;                   //!< Task options as passed by osTaskCreate()
    UINT8           OSTCBPrio;                  //!< Task priority (0 == highest)
    
    UINT32          OSTCBDly;                   //!< Ticks to pend task.

    OS_STK         *OSTCBStkPtr;                //!< Pointer to current TOP of stack

    OS_LIST_NODE    OSTCBList;                  //!< TCB list node.

    OS_WAIT_NODE   *OSTCBWaitNode;
    
    OS_MUTEX       *OSTCBOwnMutex;
    
    UINT16          OSTCBTimeSlice;
    
#if OS_TASK_PROFILE_EN > 0u
    UINT8           OSTCBStatus;
    UINT16          OSTCBStkSize;               //!< Size of task stack (in number of stack elements)
    UINT16          OSTCBStkUsed;               //!< Number of BYTES used from the stack
    OS_STK         *OSTCBStkBase;               //!< Base address of the task stack
    UINT32          OSTCBCtxSwCtr;              //!< Number of times the task was switched in
    UINT32          OSTCBCyclesTot;             //!< Total number of ticks the task has been running
    UINT32          OSTCBCyclesStart;           //!< Snapshot of tick at start of task
#endif
};

/*!
 *! FLAG DATA
 */
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
typedef struct {
    BOOL         OSFlagAutoReset;
    BOOL         OSFlagStatus;
    OS_LIST_NODE    OSWaitList;
} OS_FLAG_INFO;
#endif

/*!
 *! MUTUAL EXCLUSION SEMAPHORE DATA
 */
#if (OS_MUTEX_EN) && (OS_MAX_MUTEXES > 0u)
typedef struct {
    OS_TCB         *OSOwnerTCB;
    UINT8           OSOwnerPrio;
    UINT8           OSCeilingPrio;
    OS_LIST_NODE    OSWaitList;
    BOOL         OSValue;                    //!< Mutex value (OS_FALSE = used, TRUE = available)
} OS_MUTEX_INFO;
#endif

/*!
 *! SEMAPHORE DATA
 */
#if (OS_SEMP_EN) && (OS_MAX_SEMAPHORES > 0u)
typedef struct {
    UINT16          OSCnt;                      //!< Semaphore count
    OS_LIST_NODE    OSWaitList;
} OS_SEM_INFO;
#endif

/*!
 *! GLOBAL VARIABLES
 */
#if (OS_SEMP_EN) && (OS_MAX_SEMAPHORES > 0u)
OS_EXT  OS_LIST_NODE   *osSempFreeList;                     //!< Pointer to list of free semaphore control blocks
OS_EXT  OS_SEMP         osSempFreeTbl[OS_MAX_SEMAPHORES];   //!< Table of semaphore control blocks
#endif

#if (OS_MUTEX_EN) && (OS_MAX_MUTEXES > 0u)
OS_EXT  OS_LIST_NODE   *osMutexFreeList;                    //!< Pointer to list of free mutex control blocks
OS_EXT  OS_MUTEX        osMutexFreeTbl[OS_MAX_MUTEXES];     //!< Table of mutex control blocks
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
OS_EXT  OS_LIST_NODE   *osFlagFreeList;                     //!< Pointer to list of free flag control blocks
OS_EXT  OS_FLAG         osFlagFreeTbl[OS_MAX_FLAGS];        //!< Table of flag control blocks
#endif

OS_EXT  OS_LIST_NODE   *osTCBFreeList;                                  //!< List of free TCBs
OS_EXT  OS_TCB          osTCBFreeTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];    //!< Table of free TCBs

OS_EXT  OS_LIST_NODE    osPndList;                          //!< Doubly linked list of active task's TCB

OS_EXT  OS_PRIO         osRdyGrp;                           //!< Ready bitmap
OS_EXT  OS_PRIO         osRdyTbl[OS_BITMAP_TBL_SIZE];
OS_EXT  OS_LIST_NODE    osRdyList[OS_MAX_PRIO_LEVELS];      //!< Table of pointers to TCB of active task

OS_EXT  UINT32      osTaskCtr;

OS_EXT  UINT16      osCurTimeSlice;
OS_EXT  OS_TCB     *osTCBCur;                       //!< Pointer to currently running TCB
OS_EXT  OS_TCB     *osTCBNextRdy;                   //!< Pointer to highest priority TCB Ready-to-Run

OS_EXT  UINT8       osIntNesting;                   //!< Interrupt nesting level
OS_EXT  UINT8       osLockNesting;                  //!< Multitasking lock nesting level

OS_EXT  BOOL     osRunning;                      //!< Flag indicating that kernel is running

#if OS_STAT_EN > 0u
OS_EXT  UINT32      osCtxSwCtr;                     //!< Counter of number of context switches
OS_EXT  UINT32      osIdleCtrMax;                   //!< Max. value that idle ctr can take in 1 sec.
OS_EXT  UINT8       osCPUUsage;                     //!< Percentage of CPU used
OS_EXT  BOOL     osStatRunning;                  //!< Flag indicating that the statistic task is running
OS_EXT  OS_STK      osTaskStatStk[OS_TASK_STAT_STK_SIZE];   //!< Statistics task stack
#endif

OS_EXT  volatile UINT32 osIdleCtr;                  //!< Idle counter
OS_EXT  OS_STK      osTaskIdleStk[OS_TASK_IDLE_STK_SIZE];   //!< Idle task stack



/*!
 *!                     APPLICATION FUNCTION PROTOTYPES
 */

/*!
 *! EVENT FLAGS MANAGEMENT
 */
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)

OS_ERR      osFlagCreate           (OS_HANDLE      *pFlagHandle,
                                    BOOL         init,
                                    BOOL         manual);

#if OS_FLAG_DEL_EN > 0u
OS_ERR      osFlagDelete           (OS_HANDLE       hFlag,
                                    UINT8           opt);
#endif

OS_ERR      osFlagPend             (OS_HANDLE       hFlag,
                                    UINT32          timeout);

OS_ERR      osFlagSet              (OS_HANDLE       hFlag);

OS_ERR      osFlagReset            (OS_HANDLE       hFlag);

#if OS_FLAG_QUERY_EN > 0u
OS_ERR      osFlagQuery            (OS_HANDLE       hFlag,
                                    OS_FLAG_INFO   *p_flag_data);
#endif

#endif

/*!
 *! MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
 */
#if (OS_MUTEX_EN > 0u) && (OS_MAX_MUTEXES > 0u)

OS_ERR      osMutexCreate          (OS_HANDLE      *pMutexHandle,
                                    UINT8           ceilingPrio);

#if OS_MUTEX_DEL_EN > 0u
OS_ERR      osMutexDelete          (OS_HANDLE       hMutex,
                                    UINT8           opt);
#endif

OS_ERR      osMutexPend            (OS_HANDLE       hMutex,
                                    UINT32          timeout);

OS_ERR      osMutexPost            (OS_HANDLE       hMutex);

#if OS_MUTEX_QUERY_EN > 0u
OS_ERR      osMutexQuery           (OS_HANDLE       hMutex,
                                    OS_MUTEX_INFO  *pInfo);
#endif

#endif

/*!
 *! SEMAPHORE MANAGEMENT
 */
#if (OS_SEMP_EN > 0u) && (OS_MAX_SEMAPHORES > 0u)

OS_ERR      osSemCreate            (OS_HANDLE      *pSemaphoreHandle,
                                    UINT16          cnt);

#if OS_SEMP_DEL_EN > 0u
OS_ERR      osSemDelete            (OS_HANDLE       hSemaphore,
                                    UINT8           opt);
#endif

OS_ERR      osSemPend              (OS_HANDLE       hSemaphore,
                                    UINT32          timeout);

#if OS_SEMP_PEND_ABORT_EN > 0u
OS_ERR      osSemPendAbort         (OS_HANDLE       hSemaphore,
                                    UINT8           opt);
#endif

OS_ERR      osSemPost              (OS_HANDLE       hSemaphore,
                                    UINT16          cnt);

#if OS_SEMP_SET_EN > 0u
OS_ERR      osSemSet               (OS_HANDLE       hSemaphore,
                                    UINT16          cnt);
#endif

#if OS_SEMP_QUERY_EN > 0u
OS_ERR      osSemQuery             (OS_HANDLE       hSemaphore,
                                    OS_SEM_INFO    *pInfo);
#endif

#endif

/*!
 *! TASK MANAGEMENT
 */
OS_ERR      osTaskCreate           (OS_HANDLE      *pHandle,
                                    OS_TASK        *task,
                                    void           *parg,
                                    UINT8           prio,
                                    OS_STK         *pstk,
                                    UINT32          stkSize,
                                    UINT8           opt);

#if OS_TASK_CHANGE_PRIO_EN > 0u
OS_ERR      osTaskChangePrio       (OS_HANDLE       handle,
                                    UINT8           newprio);
#endif

/*!
 *! TIME MANAGEMENT
 */
void        osTimeDelay            (UINT32          ticks);

void        osTimeTick             (void);

/*!
 *! MISCELLANEOUS
 */
void        osInit                 (void);

void        osIntEnter             (void);
void        osIntExit              (void);

#if OS_SCHED_LOCK_EN > 0u
void        osLockSched            (void);
void        osUnlockSched          (void);
#endif

void        osStart                (void);

#if OS_STAT_EN > 0u
void        osStatInit             (void);
#endif

UINT16      osVersion              (void);

#define     osEnterCriticalSection()     OSEnterCriticalSection()
#define     osExitCriticalSection()      OSExitCriticalSection()

/*!
 *!                        TARGET SPECIFIC FUNCTION PROTOTYPES
 *! Target Specific interface and hook functions. Those functions will be called
 *! by os and should be implemented by you.
 */
void        OSStartHighRdy         (void);
void        OSIntCtxSw             (void);
void        OSCtxSw                (void);

OS_STK     *OSTaskStkInit          (OS_STK          *ptos,
                                    void            *wrap,
                                    void            *task,
                                    void            *parg);

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

/*!
 *!                       INTERNAL FUNCTION PROTOTYPES
 *!              (Your application MUST NOT call these functions)
 */

/*!
 *! OS LIST INTERFACE
 */
void        os_list_init_head      (OS_LIST_NODE   *list);
void        os_list_add            (OS_LIST_NODE   *node,
                                    OS_LIST_NODE   *head);
void        os_list_del            (OS_LIST_NODE   *entry);

bool        OS_ObjPoolFree         (OS_LIST_NODE  **ppObj,
                                    void           *pobj);
void       *OS_ObjPoolNew          (OS_LIST_NODE  **ppObj);

/*!
 *! OS SCHEDULE INTERFACE
 */
void        OS_ScheduleReadyTask   (OS_TCB         *ptcb);
void        OS_ScheduleUnreadyTask (OS_TCB         *ptcb);
void        OS_SchedulePendTask    (OS_TCB         *ptcb,
                                    UINT32          ticks);
void        OS_ScheduleUnpendTask  (OS_TCB         *ptcb);
void        OS_ScheduleChangePrio  (OS_TCB         *ptcb,
                                    UINT8           newprio);
void        OS_SchedulePrio        (void);

void        OS_Schedule            (void);

#if (OS_STAT_TASK_STK_CHK_EN > 0u)
void        OS_TaskStkChk          (OS_TCB         *ptcb);
#endif

void        OS_TCBInit             (OS_TCB         *ptcb,
                                    UINT8           prio,
                                    OS_STK         *psp,
                                    OS_STK         *pstk,
                                    UINT32          stk_size,
                                    UINT8           opt);

OS_TCB     *OS_EventTaskRdy        (void           *pecb,
                                    UINT8           pend_stat);

void        OS_EventTaskWait       (void           *pecb,
                                    OS_WAIT_NODE   *pnode,
                                    UINT32          timeout);

void        OS_EventTaskRemove     (OS_TCB         *ptcb);

void        OS_MemClr              (UINT8          *pdest,
                                    UINT32          size);

void        OS_MemCopy             (UINT8          *pdest,
                                    UINT8          *psrc,
                                    UINT32          size);

/*!
 *! LOOK FOR MISSING AND ERROR #define CONSTANTS
 *
 *! This section is used to generate ERROR messages at compile time if certain #define constants are
 *! MISSING or ERROR in OS_CFG.H.  This allows you to quickly determine the source of the error.
 *
 *! You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
 *! compile time error.
 */

/*!
 *!                                             EVENT FLAGS
 */
#ifndef OS_FLAG_EN
#   error "OS_CFG.H, Missing OS_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
#   ifndef OS_MAX_FLAGS
#       error "OS_CFG.H, Missing OS_MAX_FLAGS: Max. number of Event Flag Groups in your application"
#   else
#       if  OS_MAX_FLAGS > 65500u
#           error "OS_CFG.H, OS_MAX_FLAGS must be <= 65500"
#       endif
#   endif
#   ifndef  OS_FLAG_DEL_EN
#       error "OS_CFG.H, Missing OS_FLAG_DEL_EN: Include code for osFlagDelete()"
#   endif
#   ifndef  OS_FLAG_QUERY_EN
#       error "OS_CFG.H, Missing OS_FLAG_QUERY_EN: Include code for osFlagQuery()"
#   endif
#endif

/*!
 *!                                     MUTUAL EXCLUSION SEMAPHORES
 */
#ifndef OS_MUTEX_EN
#   error "OS_CFG.H, Missing OS_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#else
#   ifndef  OS_MUTEX_DEL_EN
#       error "OS_CFG.H, Missing OS_MUTEX_DEL_EN: Include code for osMutexDelete()"
#   endif
#   ifndef  OS_MUTEX_QUERY_EN
#       error "OS_CFG.H, Missing OS_MUTEX_QUERY_EN: Include code for osMutexQuery()"
#   endif
#endif

/*!
 *!                                             SEMAPHORES
 */
#ifndef OS_SEMP_EN
#   error "OS_CFG.H, Missing OS_SEMP_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#else
#   ifndef  OS_SEMP_DEL_EN
#       error "OS_CFG.H, Missing OS_SEMP_DEL_EN: Include code for osSemDelete()"
#   endif
#   ifndef  OS_SEMP_PEND_ABORT_EN
#       error "OS_CFG.H, Missing OS_SEMP_PEND_ABORT_EN: Include code for osSemPendAbort()"
#   endif
#   ifndef  OS_SEMP_QUERY_EN
#       error "OS_CFG.H, Missing OS_SEMP_QUERY_EN: Include code for osSemQuery()"
#   endif
#   ifndef  OS_SEMP_SET_EN
#       error "OS_CFG.H, Missing OS_SEMP_SET_EN: Include code for osSemSet()"
#   endif
#endif

/*!
 *!                                           TASK MANAGEMENT
 */
#ifndef OS_MAX_PRIO_LEVELS
#   error "OS_CFG.H, Missing OS_MAX_PRIO_LEVELS: Max. levels of priority in your application"
#else
#   if  OS_MAX_PRIO_LEVELS <  2u
#       error "OS_CFG.H, OS_MAX_PRIO_LEVELS must be >= 2"
#   endif
#   if  OS_MAX_PRIO_LEVELS >  256u
#       error "OS_CFG.H, OS_MAX_PRIO_LEVELS must be <= 256"
#   endif
#endif

#ifndef OS_TASK_IDLE_STK_SIZE
#   error "OS_CFG.H, Missing OS_TASK_IDLE_STK_SIZE: Idle task stack size"
#endif

#ifndef OS_STAT_EN
#   error "OS_CFG.H, Missing OS_STAT_EN: Enable (1) or Disable(0) the statistics task"
#endif

#ifndef OS_TASK_STAT_STK_SIZE
#   error "OS_CFG.H, Missing OS_TASK_STAT_STK_SIZE: Statistics task stack size"
#endif

#ifndef OS_STAT_TASK_STK_CHK_EN
#   error "OS_CFG.H, Missing OS_STAT_TASK_STK_CHK_EN: Check task stacks from statistics task"
#endif

#ifndef OS_TASK_CHANGE_PRIO_EN
#   error "OS_CFG.H, Missing OS_TASK_CHANGE_PRIO_EN: Include code for osTaskChangePrio()"
#endif

/*!
 *!                                           TIME MANAGEMENT
 */
#ifndef OS_TICKS_PER_SEC
#   error "OS_CFG.H, Missing OS_TICKS_PER_SEC: Sets the number of ticks in one second"
#else
#   if  OS_TICKS_PER_SEC == 0
#       error "OS_CFG.H, OS_TICKS_PER_SEC must be > 0"
#   endif
#endif

/*!
 *!                                            MISCELLANEOUS
 */
#ifndef OS_ARG_CHK_EN
#   error "OS_CFG.H, Missing OS_ARG_CHK_EN: Enable (1) or Disable (0) argument checking"
#endif

#ifndef OS_CPU_HOOKS_EN
#   error "OS_CFG.H, Missing OS_CPU_HOOKS_EN: OS hooks are found in the processor port files when 1"
#endif

#ifndef OS_DEBUG_EN
#   error "OS_CFG.H, Missing OS_DEBUG_EN: Allows you to include variables for debugging or not"
#endif

#ifndef OS_MAX_PRIO_LEVELS
#   error "OS_CFG.H, Missing OS_MAX_PRIO_LEVELS: Defines the lowest priority that can be assigned"
#endif

#ifndef OS_MAX_SEMAPHORES
#   error "OS_CFG.H, Missing OS_MAX_SEMAPHORES: Max. number of event control blocks in your application"
#else
#   if  OS_MAX_SEMAPHORES > 65535u
#       error "OS_CFG.H, OS_MAX_SEMAPHORES must be <= 65535"
#   endif
#endif

#ifndef OS_SCHED_LOCK_EN
#   error "OS_CFG.H, Missing OS_SCHED_LOCK_EN: Include code for osLockSched() and osUnlockSched()"
#endif

#ifndef OS_TASK_SW_HOOK_EN
#   error "OS_CFG.H, Missing OS_TASK_SW_HOOK_EN: Allows you to include the code for OSTaskSwHook() or not"
#endif

#ifndef OS_TIME_TICK_HOOK_EN
#   error "OS_CFG.H, Missing OS_TIME_TICK_HOOK_EN: Allows you to include the code for OSTimeTickHook() or not"
#endif

/*!
 *!                                         SAFETY CRITICAL USE
 */
#ifdef SAFETY_CRITICAL_RELEASE

#if    OS_ARG_CHK_EN == 0u
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
#   if OS_FLAG_DEL_EN > 0
#       error "OS_CFG.H, OS_FLAG_DEL_EN must be disabled for safety-critical release code"
#   endif
#endif

#if    OS_MUTEX_EN > 0u
#   if OS_MUTEX_DEL_EN > 0u
#       error "OS_CFG.H, OS_MUTEX_DEL_EN must be disabled for safety-critical release code"
#   endif
#endif

#if    OS_SEMP_EN > 0u
#   if OS_SEMP_DEL_EN > 0u
#       error "OS_CFG.H, OS_SEMP_DEL_EN must be disabled for safety-critical release code"
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