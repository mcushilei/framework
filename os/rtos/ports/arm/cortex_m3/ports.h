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

#ifndef  __KERNEL_CORTEX_M3_PORTS_H__
#define  __KERNEL_CORTEX_M3_PORTS_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\os_cpu_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define INT_PRIO_MASK               (1u << 3)

#if OS_CRITICAL_METHOD == 3
    #define  OS_ENTER_CRITICAL()    cpu_sr = CALL_SVC2(1, INT_PRIO_MASK)  //!< disable interrupt.
    #define  OS_EXIT_CRITICAL()     CALL_SVC2(1, cpu_sr)    //!< enable interrupt.
#else
    #define OS_ENTER_CRITICAL()     CALL_SVC1(1)            //!< disable interrupt.
    #define OS_EXIT_CRITICAL()      CALL_SVC1(2)            //!< enable interrupt.
#endif

#define	SVC_TO_USER()               CALL_SVC2(2, 1)
#define	SVC_TO_PRIVILEGE()          CALL_SVC2(2, 0)
#define	SVC_DO_NOTHING()            CALL_SVC3()             /* 触发一次SVC中断，但什么都没做*/

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void DISABLE_ALL_INT(void);
extern INT32U CALL_SVC1(INT32U x);
extern INT32U CALL_SVC2(INT32U x, INT32U y);
extern INT32U CALL_SVC3(void);




#endif
