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


#ifndef __SERVICE_UCOS_OS_H__
#define __SERVICE_UCOS_OS_H__

/*============================ INCLUDES ======================================*/
#include ".\ucos_ii\ucos_ii.h"

/*============================ MACROS ========================================*/
#define INFINITE                            (0ul)


#define OS_CRITICAL_TYPE                    uint8_t
#define OS_CRITICAL_INIT(__CRITICAL)        
#define OS_CRITICAL_DEINIT(__CRITICAL)      
#define OS_CRITICAL_ENTER(__CRITICAL)       OS_ENTER_CRITICAL(__CRITICAL)
#define OS_CRITICAL_EXIT(__CRITICAL)        OS_EXIT_CRITICAL(__CRITICAL)


#define OS_MUTEX_TYPE                       OS_EVENT *

#define OS_MUTEX_CREATE(__MUTEX, __OWNER) do {\
                if (!(__OWNER)) {\
                    (__MUTEX) = OSSemCreate(0);\
                } else {\
                    (__MUTEX) = OSSemCreate(1);\
                }\
            } while (0)

#define OS_MUTEX_DELETE(__MUTEX) do {\
                uint8_t chOSError;\
                OSSemDel(__MUTEX, OS_DEL_ALWAYS, &chOSError);\
            } while (0)

#define OS_MUTEX_WAIT(__MUTEX, __TIME, __RES)  do {\
                OSSemPend(__MUTEX, __TIME, &(uint8_t)__RES);\
            } while (0)

#define OS_MUTEX_RELEASE(__MUTEX, __RES)   do {\
                __RES = OSSemPost(__MUTEX);\
            } while (0)


#define OS_EVENT_TYPE                       OS_FLAG_GRP *

#define OS_EVENT_CREATE(__EVENT, __BMANUAL, __BINITVAL) do {\
                uint8_t chOSError;\
                OS_FLAGS flags = 0;\
                if (__BMANUAL) {\
                    flags = 0x80u;\
                } else {\
                    flags = 0u;\
                }\
                if (__BINITVAL) {\
                    (__EVENT) = OSFlagCreate(flags | 0x01u, &chOSError);\
                } else {\
                    (__EVENT) = OSFlagCreate(flags, &chOSError);\
                }\
            } while (0)

#define OS_EVENT_DELETE(__EVENT)    do {\
                uint8_t chOSError;\
                OSFlagDel(__EVENT, OS_DEL_ALWAYS, &chOSError);\
            } while (0)

#define OS_EVENT_SET(__EVENT, __RES)   do {\
                OSFlagPost(__EVENT, 0x01u, OS_FLAG_SET, &(uint8_t)__RES);\
            } while (0)

#define OS_EVENT_RESET(__EVENT, __RES) do {\
                OSFlagPost(__EVENT, 0x01u, OS_FLAG_CLR, &(uint8_t)__RES);\
            } while (0)

#define OS_EVENT_WAIT(__EVENT, __TIME, __RES) do {\
                uint8_t chOSError;\
                OS_FLAGS flags = 0;\
                OSFlagPend(__EVENT, 0x01u, OS_FLAG_WAIT_SET_ANY, __TIME, &(uint8_t)__RES);\
                flags = OSFlagAccept(__EVENT, 0x80u, OS_FLAG_WAIT_SET_ANY, __TIME, &(uint8_t)__RES);\
                if (flags & 0x80u) {\
                    OSFlagPost(__EVENT, 0x01u, OS_FLAG_CLR, &chOSError);\
                }\
            } while (0)

 
#define OS_TIME_GET()                   OSTimeGet()

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
