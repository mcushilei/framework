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


#ifndef __KERNEL_OS_H__
#define __KERNEL_OS_H__

/*============================ INCLUDES ======================================*/
#include ".\source\os.h"

/*============================ MACROS ========================================*/
#define INFINITE                            (0ul)


#define OS_CRITICAL_TYPE                    uint8_t
#define OS_CRITICAL_INIT(__CRITICAL)        
#define OS_CRITICAL_DEINIT(__CRITICAL)      
#define OS_CRITICAL_ENTER(__CRITICAL)       OS_ENTER_CRITICAL(__CRITICAL)
#define OS_CRITICAL_EXIT(__CRITICAL)        OS_EXIT_CRITICAL(__CRITICAL)


#define OS_MUTEX_TYPE                       OS_HANDLE

#define OS_MUTEX_CREATE(__MUTEX, __OWNER) do {\
                osMutexCreate(&__MUTEX);\
            } while (0)

#define OS_MUTEX_DELETE(__MUTEX) do {\
                osMutexDelete(__MUTEX, OS_DEL_ALWAYS);\
            } while (0)

#define OS_MUTEX_WAIT(__MUTEX, __TIME, __RES)  do {\
                osMutexPend(__MUTEX, __TIME);\
            } while (0)

#define OS_MUTEX_RELEASE(__MUTEX)   do {\
                osMutexPost(__MUTEX);\
            } while (0)


#define OS_FLAG_TYPE                        OS_HANDLE

#define OS_FLAG_CREATE(__EVENT, __BMANUAL, __BINITVAL) do {\
                osFlagCreate(&__EVENT, __BINITVAL, __BMANUAL);\
            } while (0)

#define OS_FLAG_DELETE(__EVENT)    do {\
                osFlagDelete(__EVENT, OS_DEL_ALWAYS);\
            } while (0)

#define OS_FLAG_SET(__EVENT)   do {\
                osFlagSet(__EVENT);\
            } while (0)

#define OS_FLAG_RESET(__EVENT) do {\
                osFlagReset(__EVENT);\
            } while (0)

#define OS_FLAG_WAIT(__EVENT, __TIME, __RES) do {\
                __RES = osFlagPend(__EVENT, __TIME);\
            } while (0)

 
#define OS_TIME_GET()                   OSTimeGet()

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
