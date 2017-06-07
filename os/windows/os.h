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


#ifndef __OS_WINDOWS_OS_C__
#ifndef __OS_WINDOWS_OS_H__
#define __OS_WINDOWS_OS_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\api\os.h"
#include ".\api\com\com.h"
#include ".\api\block\block.h"

/*============================ MACROS ========================================*/
enum {
    OS_ERR_NONE                     = 0u,

    OS_ERR_EVENT_TYPE               = 1u,
    OS_ERR_PDATA_NULL               = 2u,
    OS_ERR_INVALID_HANDLE           = 3u,
    OS_ERR_INVALID_OPT              = 4u,
    OS_ERR_DEL_ISR                  = 5u,
    OS_ERR_CREATE_ISR               = 6u,
    OS_ERR_INVALID_TASK_HANDLE      = 7u,

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


typedef uint8_t OS_ERR;
typedef HANDLE  OS_HANDLE;

#define OS_INFINITE                         INFINITE

#define OS_CRITICAL_DEFINE(__ATOM)          CRITICAL_SECTION __ATOM;
#define OS_CRITICAL_INIT(__ATOM)            do {InitializeCriticalSectionAndSpinCount(&(__ATOM), 0x00000400);} while (0)
#define OS_CRITICAL_DEINIT(__ATOM)          do {DeleteCriticalSection(&(__ATOM));} while (0)
#define OS_CRITICAL_ENTER(__ATOM)           EnterCriticalSection(&(__ATOM))
#define OS_CRITICAL_EXIT(__ATOM)            LeaveCriticalSection(&(__ATOM))


#define OS_SEMAPHORE_TYPE                   OS_HANDLE
#define OS_SEMAPHORE_CREATE(__SEM, __CNT)   osSemCreate(&__SEM, __CNT)
#define OS_SEMAPHORE_DELETE(__SEM)          osSemDelete(__SEM, 0)
#define OS_SEMAPHORE_RELEASE(__SEM)         osSemPost(__SEM, 1u)
#define OS_SEMAPHORE_WAIT(__SEM, __TIME)    osSemPend(__SEM, __TIME)


#define OS_MUTEX_TYPE                       OS_HANDLE
#define OS_MUTEX_CREATE(__MUTEX)            osMutexCreate(&__MUTEX, 0)
#define OS_MUTEX_DELETE(__MUTEX)            osMutexDelete(__MUTEX, 0)
#define OS_MUTEX_RELEASE(__MUTEX)           osMutexPost(__MUTEX)
#define OS_MUTEX_WAIT(__MUTEX, __TIME)      osMutexPend(__MUTEX, __TIME)


#define OS_FLAG_TYPE                        OS_HANDLE
#define OS_FLAG_CREATE(__FLAG, __BMANUAL, __BINITVAL) osFlagCreate(&__FLAG, __BINITVAL, __BMANUAL)
#define OS_FLAG_DELETE(__FLAG)              osFlagDelete(__FLAG, 0)
#define OS_FLAG_SET(__FLAG)                 osFlagSet(__FLAG)
#define OS_FLAG_RESET(__FLAG)               osFlagReset(__FLAG)
#define OS_FLAG_WAIT(__FLAG, __TIME)        osFlagPend(__FLAG, __TIME)


#define OS_TASK_SLEEP(__T)                  osTimeDelay(__T)

#define OS_TASK_DEFINE_BEGIN(__TASK_NAME, __ARG_NAME)   DWORD WINAPI __TASK_NAME(void *__ARG_NAME) {
#define OS_TASK_DEFINE_END(__TASK_NAME)                 return 0;}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern void     osTimeDelay        (UINT32          ticks);

extern OS_ERR   osFlagCreate       (OS_HANDLE      *pFlagHandle,
                                    BOOLEAN         init,
                                    BOOLEAN         manual);

extern OS_ERR   osFlagDelete       (OS_HANDLE       hFlag,
                                    UINT8           opt);

extern OS_ERR   osFlagPend         (OS_HANDLE       hFlag,
                                    UINT32          timeout);

extern OS_ERR   osFlagSet          (OS_HANDLE       hFlag);

extern OS_ERR   osFlagReset        (OS_HANDLE       hFlag);


extern OS_ERR   osMutexCreate      (OS_HANDLE      *pMutexHandle,
                                    UINT8           ceilingPrio);

extern OS_ERR   osMutexDelete      (OS_HANDLE       hMutex,
                                    UINT8           opt);

extern OS_ERR   osMutexPend        (OS_HANDLE       hMutex,
                                    UINT32          timeout);

extern OS_ERR   osMutexPost        (OS_HANDLE       hMutex);


extern OS_ERR   osSemCreate        (OS_HANDLE      *pSemaphoreHandle,
                                    UINT16          cnt);

extern OS_ERR   osSemDelete        (OS_HANDLE       hSemaphore,
                                    UINT8           opt);

extern OS_ERR   osSemPend          (OS_HANDLE       hSemaphore,
                                    UINT32          timeout);


extern OS_ERR   osSemPost          (OS_HANDLE       hSemaphore,
                                    UINT16          cnt);

extern OS_ERR   osSemSet           (OS_HANDLE       hSemaphore,
                                    UINT16          cnt);


#endif
#endif
/* EOF */
