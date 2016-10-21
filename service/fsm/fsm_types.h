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


#ifndef __FSM_TYPES_H__
#define __FSM_TYPES_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef volatile struct _task   task_ctrl_block_t;
typedef void state_func_t(void *pArg);

DEF_STRUCTURE(task_stack_item_t)
    state_func_t        *fnState;       //!< routine
    void                *pArg;          //!< argument
END_DEF_STRUCTURE(task_stack_item_t);

DEF_STRUCTURE(event_t)
    bool                bSignal;        //!< signal
    task_ctrl_block_t   *ptHead;        //!< task item  
    task_ctrl_block_t   *ptTail;
    bool                bManualReset;   //!< manual reset flag
END_DEF_STRUCTURE(event_t)

struct _task
{
    task_ctrl_block_t   *pNext;

    task_stack_item_t   *pStack;            //!< task call stack
    uint8_t             chStackSize;        //!< stack size
    uint8_t             chStackLevel;       //!< stack pointer
    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    event_t             *pObject;           //!< target event
#endif
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __FSM_TYPES_H__
/* EOF */
