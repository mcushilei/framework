/*******************************************************************************
 *  Copyright(C)2016 by Dreistein<mcu_shilei@hotmail.com>                     *
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

//! \note do not move this pre-processor statement to other places
#include "..\app_cfg.h"

#ifndef __SOFTTIMER_APP_CFG_H__
#define __SOFTTIMER_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define SOFTTIMER_MAX_TIMERS    (32u)

#ifdef WIN32
#define __SOFTTIMER_SAFE_ATOME_CODE(...) {\
            OS_CRITICAL_ENTER(softtimerCriticalSection);\
            __VA_ARGS__\
            OS_CRITICAL_EXIT(softtimerCriticalSection);\
        }
#elif defined(__OS_RTOS__)
#define __SOFTTIMER_SAFE_ATOME_CODE(...) {\
            OS_CPU_SR  cpu_sr = 0u;\
            osEnterCriticalSection(cpu_sr);\
            __VA_ARGS__\
            osExitCriticalSection(cpu_sr);\
        }
#else
#define __SOFTTIMER_SAFE_ATOME_CODE(...)    __VA_ARGS__
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __SOFTTIMER_APP_CFG_H__
/* EOF */
