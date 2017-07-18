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


#ifndef __XOS_OS_H__
#define __XOS_OS_H__

/*============================ INCLUDES ======================================*/
#include ".\source\os.h"

/*============================ MACROS ========================================*/

#define OS_CRITICAL_DEFINE(__CRITICAL)      OS_CPU_SR   __CRITICAL;
#define OS_CRITICAL_INIT(__CRITICAL)        do {} while (0)
#define OS_CRITICAL_DEINIT(__CRITICAL)      do {} while (0)
#define OS_CRITICAL_ENTER(__CRITICAL)       osEnterCriticalSection(__CRITICAL)
#define OS_CRITICAL_EXIT(__CRITICAL)        osExitCriticalSection(__CRITICAL)


#define OS_SEMAPHORE_TYPE                   OS_HANDLE
#define OS_SEMAPHORE_CREATE(__SEM, __CNT)   osSemCreate(&__SEM, __CNT)
#define OS_SEMAPHORE_DELETE(__SEM)          osSemDelete(__SEM, OS_DEL_ALWAYS)
#define OS_SEMAPHORE_RELEASE(__SEM)         osSemPost(__SEM, 1u)
#define OS_SEMAPHORE_WAIT(__SEM, __TIME)    osSemPend(__SEM, __TIME)


#define OS_MUTEX_TYPE                       OS_HANDLE
#define OS_MUTEX_CREATE(__MUTEX)            osMutexCreate(&__MUTEX, OS_TASK_LOWEST_PRIO)
#define OS_MUTEX_DELETE(__MUTEX)            osMutexDelete(__MUTEX, OS_DEL_ALWAYS)
#define OS_MUTEX_RELEASE(__MUTEX)           osMutexPost(__MUTEX)
#define OS_MUTEX_WAIT(__MUTEX, __TIME)      osMutexPend(__MUTEX, __TIME)


#define OS_FLAG_TYPE                        OS_HANDLE
#define OS_FLAG_CREATE(__FLAG, __BMANUAL, __BINITVAL) osFlagCreate(&__FLAG, __BINITVAL, __BMANUAL)
#define OS_FLAG_DELETE(__FLAG)              osFlagDelete(__FLAG, OS_DEL_ALWAYS)
#define OS_FLAG_SET(__FLAG)                 osFlagSet(__FLAG)
#define OS_FLAG_RESET(__FLAG)               osFlagReset(__FLAG)
#define OS_FLAG_WAIT(__FLAG, __TIME)        osFlagPend(__FLAG, __TIME)


#define OS_TASK_SLEEP(__T)                  osTimeDelay(__T)
                                                
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */