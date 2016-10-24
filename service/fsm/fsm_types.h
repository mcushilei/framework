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


#ifndef __FSM_TYPES_H__
#define __FSM_TYPES_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"


/*============================ MACROS ========================================*/
#define FSM_SIGNAL_FLAG_BIT         (1u << 0)
#define FSM_SIGNAL_MANUAL_RESET_BIT (1u << 1)

#define FSM_OBJ_FLAG_REG_BIT        (1u << 0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef volatile struct _task   fsm_tcb_t;
typedef void state_func_t(void *pArg);

DEF_STRUCTURE(task_stack_item_t)
    state_func_t        *fnState;       //!< routine
    void                *pArg;          //!< argument
END_DEF_STRUCTURE(task_stack_item_t);

typedef enum {
    FSM_OBJ_TYPE_INVALID = 0,
    FSM_OBJ_TYPE_EVENT,
    FSM_OBJ_TYPE_MUTEX,
    FSM_OBJ_TYPE_SEM,
} fsm_obj_type_em_t;

typedef enum {
    FSM_ERR_NONE = 0,
    FSM_ERR_NULL_PTR,
    FSM_ERR_OBJ_POOL_EMPTY,
    FSM_ERR_TASK_NO_MORE_TCB,
    FSM_ERR_TASK_FULL,
} fsm_err_em_t;

DEF_STRUCTURE(fsm_obj_t)
    uint8_t             chObjType;
    uint8_t             chObjFlag;
    fsm_obj_t           *ptObjNext;
    fsm_tcb_t           *ptTCBHead;
    fsm_tcb_t           *ptTCBTail;
END_DEF_STRUCTURE(fsm_obj_t)

DEF_STRUCTURE(fsm_event_t)
    fsm_obj_t;
    uint8_t             chSignal;        //!< signal
END_DEF_STRUCTURE(fsm_event_t)

struct _task
{
    fsm_tcb_t           *pNext;
    
    uint8_t             chFlag;
    uint8_t             chStatus;
    
    uint16_t            hwDelay;

    uint8_t             chStackSize;    //!< stack size
    uint8_t             chStackLevel;   //!< stack pointer
    task_stack_item_t   *pStack;        //!< task call stack
    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    fsm_obj_t           *ptObject;       //!< target event
#endif
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __FSM_TYPES_H__
/* EOF */
