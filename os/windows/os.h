/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
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


#ifndef __OS_WINDOWS_OS_H__
#define __OS_WINDOWS_OS_H__

/*============================ INCLUDES ======================================*/
#include ".\api\os.h"
#include ".\api\com\com.h"

/*============================ MACROS ========================================*/
#define OS_CRITICAL_TYPE                        CRITICAL_SECTION
#define OS_CRITICAL_INIT(__ATOM)                InitializeCriticalSectionAndSpinCount(&(__ATOM), 0x00000400)
#define OS_CRITICAL_DEINIT(__ATOM)              DeleteCriticalSection(&(__ATOM))
#define OS_CRITICAL_ENTER(__ATOM)               EnterCriticalSection(&(__ATOM))
#define OS_CRITICAL_EXIT(__ATOM)                LeaveCriticalSection(&(__ATOM))

#define OS_MUTEX_TYPE                           HANDLE
#define OS_MUTEX_CREATE(__MUTEX)                __MUTEX = CreateMutex(NULL, FALSE, NULL)
#define OS_MUTEX_DELETE(__MUTEX)                CloseHandle(__MUTEX)
#define OS_MUTEX_WAIT(__MUTEX, __TIME, __RES)   __RES = WaitForSingleObject(__MUTEX, __TIME)
#define OS_MUTEX_RELEASE(__MUTEX)               ReleaseMutex(__MUTEX)

#define OS_FLAG_TYPE                           HANDLE
#define OS_FLAG_CREATE(__FLAG, __BMANUAL, __BINITVAL)\
                                                __FLAG = CreateEvent(NULL, __BMANUAL, __BINITVAL, NULL)
#define OS_FLAG_DELETE(__FLAG)                  CloseHandle(__FLAG)
#define OS_FLAG_SET(__FLAG)                     SetEvent(__FLAG)
#define OS_FLAG_RESET(__FLAG)                   ResetEvent(__FLAG)
#define OS_FLAG_WAIT(__FLAG, __TIME, __RES)     __RES = WaitForSingleObject(__FLAG, __TIME)

#define OS_TIME_GET()                           GetTickCount()

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
