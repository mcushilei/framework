
#ifndef __OS_CFG_H__
#define __OS_CFG_H__


                                       //!< ---------------------- MISCELLANEOUS -----------------------
#define OS_ARG_CHK_EN             1u   //!< Enable (1) or Disable (0) argument checking
#define OS_CPU_HOOKS_EN           0u   //!< Hooks are found in the processor port files

#define OS_DEBUG_EN               1u   //!< Enable(1) debug variables

#define OS_MAX_PRIO_LEVELS      255u   //!< Defines the priority levels ...
                                       //!< ... MUST NEVER be higher than 255!

#define OS_MAX_EVENTS            8u   //!< Max. number of event control blocks in your application
#define OS_MAX_FLAGS             8u   //!< Max. number of Event Flag Groups    in your application
#define OS_MAX_TASKS             8u   //!< Max. number of tasks in your application, MUST be >= 2

#define OS_SCHED_LOCK_EN          1u   //!< Include code for osSchedLock() and osSchedUnlock()

#define OS_TICKS_PER_SEC        100u   //!< Set the number of ticks in one second
#define OS_TIME_TICK_HOOK_EN      1u   //!<     Include code for OSTimeTickHook()

#define OS_STAT_EN                1u   //!<     Enable (1) or Disable(0) the statistics task
#define OS_STAT_TASK_STK_CHK_EN   1u   //!<     Check task stacks from statistic task


                                       //!< --------------------- TASK STACK SIZE ----------------------
#define OS_TASK_STAT_STK_SIZE    32u   //!< Statistics task stack size (# of OS_STK wide entries)
#define OS_TASK_IDLE_STK_SIZE    32u   //!< Idle       task stack size (# of OS_STK wide entries)


                                       //!< --------------------- TASK MANAGEMENT ----------------------
#define OS_TASK_CHANGE_PRIO_EN    1u   //!<     Include code for osTaskChangePrio()
#define OS_TASK_PROFILE_EN        1u   //!<     Include variables in OS_TCB for profiling
#define OS_TASK_QUERY_EN          1u   //!<     Include code for osTaskQuery()
#define OS_TASK_SW_HOOK_EN        1u   //!<     Include code for OSTaskSwHook()


                                       //!< ----------------------- EVENT FLAGS ------------------------
#define OS_FLAG_EN                1u   //!< Enable (1) or Disable (0) code generation for EVENT FLAGS
#define OS_FLAG_DEL_EN            1u   //!<     Include code for osFlagDelete()
#define OS_FLAG_QUERY_EN          1u   //!<     Include code for osFlagQuery()


                                       //!< ---------------- MUTUAL EXCLUSION SEMAPHORES ---------------
#define OS_MUTEX_EN               1u   //!< Enable (1) or Disable (0) code generation for MUTEX
#define OS_MUTEX_DEL_EN           1u   //!<     Include code for osMutexDelete()
#define OS_MUTEX_QUERY_EN         1u   //!<     Include code for osMutexQuery()


                                       //!< ------------------------ SEMAPHORES ------------------------
#define OS_SEM_EN                 1u   //!< Enable (1) or Disable (0) code generation for SEMAPHORES
#define OS_SEM_DEL_EN             1u   //!<    Include code for osSemDelete()
#define OS_SEM_QUERY_EN           1u   //!<    Include code for osSemQuery()
#define OS_SEM_SET_EN             1u   //!<    Include code for OSSemSet()
#define OS_SEM_PEND_ABORT_EN      1u   //!<    Include code for osSemPendAbort()

//#define SAFETY_CRITICAL_RELEASE

#endif
