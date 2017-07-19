
//! \note do not move this pre-processor statement to other places
#ifndef __OS_CFG_H__
#define __OS_CFG_H__

#include ".\app_cfg.h"

#ifndef __USER_OS_CFG__

//! default configration.
                                       //!< ---------------------- MISCELLANEOUS -----------------------
#define OS_ARG_CHK_EN             1u   //!< Enable (1) or Disable (0) argument checking
#define OS_CPU_HOOKS_EN           1u   //!< Hooks are found in the processor port files

#define OS_DEBUG_EN               1u   //!< Enable(1) debug variables

#define OS_MAX_PRIO_LEVELS      5u   //!< Max. number of priority levels ...
                                       //!< ... MUST be <= 256!

#define OS_MAX_SEMAPHORES        16u   //!< Max. number of semaphore event control blocks in your application
#define OS_MAX_MUTEXES           16u   //!< Max. number of mutex     event control blocks in your application
#define OS_MAX_FLAGS             16u   //!< Max. number of flag      event control blocks in your application
#define OS_MAX_TASKS             16u   //!< Max. number of tasks in YOUR application

#define OS_TICKS_PER_SEC        100u   //!< Set the number of ticks in one second
#define OS_TIME_TICK_HOOK_EN      1u   //!<     Include code for OSTimeTickHook()

#define OS_STAT_EN                0u   //!<     Enable (1) or Disable(0) the statistics task
#define OS_STAT_TASK_STK_CHK_EN   1u   //!<     Check task stacks from statistic task


                                       //!< --------------------- TASK STACK SIZE ----------------------
#define OS_TASK_STAT_STK_SIZE    32u   //!< Statistics task stack size (# of OS_STK wide entries)
#define OS_TASK_IDLE_STK_SIZE    32u   //!< Idle       task stack size (# of OS_STK wide entries)


                                       //!< --------------------- TASK MANAGEMENT ----------------------
#define OS_TASK_CHANGE_PRIO_EN    1u   //!<     Include code for osTaskChangePrio()
#define OS_TASK_PROFILE_EN        1u   //!<     Include variables in OS_TCB for profiling
#define OS_TASK_SW_HOOK_EN        1u   //!<     Include code for OSTaskSwHook()


                                       //!< ----------------------- EVENT FLAGS ------------------------
#define OS_FLAG_EN                1u   //!< Enable (1) or Disable (0) code generation for EVENT FLAGS
#define OS_FLAG_DEL_EN            1u   //!<     Include code for osFlagDelete()
#define OS_FLAG_QUERY_EN          1u   //!<     Include code for osFlagQuery()


                                       //!< ---------------- MUTUAL EXCLUSION SEMPAPHORES ---------------
#define OS_MUTEX_EN               1u   //!< Enable (1) or Disable (0) code generation for MUTEX
#define OS_MUTEX_DEL_EN           1u   //!<     Include code for osMutexDelete()
#define OS_MUTEX_QUERY_EN         1u   //!<     Include code for osMutexQuery()


                                       //!< ------------------------ SEMPAPHORES ------------------------
#define OS_SEMP_EN                1u   //!< Enable (1) or Disable (0) code generation for SEMPAPHORES
#define OS_SEMP_DEL_EN            1u   //!<    Include code for osSemDelete()
#define OS_SEMP_QUERY_EN          1u   //!<    Include code for osSemQuery()
#define OS_SEMP_SET_EN            1u   //!<    Include code for OSSemSet()
#define OS_SEMP_PEND_ABORT_EN     1u   //!<    Include code for osSemPendAbort()

//#define SAFETY_CRITICAL_RELEASE

#endif      //!< #ifnde __USER_OS_CFG__

#endif
