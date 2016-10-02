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


#ifndef __SERVICE_OS_WINDOWS_OS_H__
#define __SERVICE_OS_WINDOWS_OS_H__

/*============================ INCLUDES ======================================*/
#include <Windows.h>

/*============================ MACROS ========================================*/
#define OS_CRITICAL_TYPE                CRITICAL_SECTION
#define OS_CRITICAL_INIT(__ATOM)        InitializeCriticalSectionAndSpinCount(__ATOM, 0x00000400)
#define OS_CRITICAL_DEINIT(__ATOM)      DeleteCriticalSection(__ATOM)
#define OS_CRITICAL_ENTER(__ATOM)       EnterCriticalSection(__ATOM)
#define OS_CRITICAL_EXIT(__ATOM)        LeaveCriticalSection(__ATOM)

#define OS_MUTEX_TYPE                           CRITICAL_SECTION
#define OS_MUTEX_CREAT(__MUTEX, __BINITVAL)     InitializeCriticalSectionAndSpinCount(__MUTEX, 0x00000400)
#define OS_MUTEX_DELET(__MUTEX)                 DeleteCriticalSection(__MUTEX)
#define OS_MUTEX_WAIT(__MUTEX, __RES)           EnterCriticalSection(__MUTEX)
#define OS_MUTEX_RELEASE(__MUTEX, __RES)        LeaveCriticalSection(__MUTEX)

#define OS_EVENT_TYPE                           HANDLE
#define OS_EVENT_INIT(__EVENT, __BMANUAL, __BINITVAL) __EVENT = CreateEvent(NULL, __BMANUAL, __BINITVAL, NULL)
#define OS_EVENT_DEINIT(__EVENT)                CloseHandle(__EVENT)
#define OS_EVENT_SET(__EVENT, __RES)            __RES = SetEvent(__EVENT)
#define OS_EVENT_RESET(__EVENT, __RES)          __RES = ResetEvent(__EVENT)
#define OS_EVENT_WAIT(__EVENT, __TIME, __RES)   __RES = WaitForSingleObject(__EVENT, __TIME)

#define OS_TIME_GET()                   GetTickCount()

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
