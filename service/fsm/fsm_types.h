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
#define FSM_EVENT_SINGNAL_BIT       (1u << 0)
#define FSM_EVENT_MANUAL_RESET_BIT  (1u << 1)

#define FSM_MUTEX_OWNED_BIT         (1u << 0)

#define FSM_OBJ_FLAG_REG_BIT        (1u << 0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum {
    FSM_ERR_NONE            = 0,
    FSM_ERR_INVALID_PARAM,
    FSM_ERR_OBJ_NO_MORE_OCB,
    FSM_ERR_OBJ_NOT_SINGLED,
    FSM_ERR_OBJ_NOT_WAITABLE,
    FSM_ERR_OBJ_TYPE_MISMATCHED,
    FSM_ERR_OPT_NOT_SUPPORT,
    FSM_ERR_TASK_NO_MORE_TCB,
    FSM_ERR_TASK_PEND_TIMEOUT,
    FSM_ERR_TASK_STACK_FULL,
    FSM_ERR_PEND_ISR,
    FSM_ERR_SEM_EXCEED,
} fsm_err_em_t;

typedef enum {
    FSM_OBJ_TYPE_INVALID    = 0,
    FSM_OBJ_TYPE_WAITABLE   = 0x80,
    FSM_OBJ_TYPE_EVENT      = 0x80,
    FSM_OBJ_TYPE_MUTEX      = 0x81,
    FSM_OBJ_TYPE_SEM        = 0x82,
    FSM_OBJ_TYPE_TIMER      = 0x83,
} fsm_obj_type_em_t;

typedef enum {
    FSM_TASK_STATUS_INVALID = 0,
    FSM_TASK_STATUS_READY,
    FSM_TASK_STATUS_PEND,
    FSM_TASK_STATUS_PEND_OK,
    FSM_TASK_STATUS_PEND_TIMEOUT,
} fsm_task_status_em_t;

typedef void state_func_t(void *pArg);

DEF_STRUCTURE(task_stack_item_t)
    state_func_t        *fnState;       //!< routine
    void                *pArg;          //!< argument
END_DEF_STRUCTURE(task_stack_item_t);

DEF_STRUCTURE(fsm_obj_t)
    uint8_t             chObjType;
    uint8_t             chObjFlag;
    
    fsm_obj_t           *ptObjNext;
END_DEF_STRUCTURE(fsm_obj_t)

DEF_STRUCTURE(fsm_tcb_t)
    fsm_tcb_t           *pNext;
    
    uint32_t            wDelay;
    
    uint8_t             chFlag;
    uint8_t             chStatus;

    uint8_t             chStackSize;    //!< stack size
    uint8_t             chStackPoint;   //!< stack pointer
    task_stack_item_t   *pStack;        //!< task call stack
    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    fsm_obj_t           *ptObject;      //!< target event
#endif
END_DEF_STRUCTURE(fsm_tcb_t)

DEF_STRUCTURE(task_queue_t)
    fsm_tcb_t           *ptTCBHead;     //!< queue head
    fsm_tcb_t           *ptTCBTail;     //!< queue tail
END_DEF_STRUCTURE(task_queue_t)

DEF_STRUCTURE(fsm_waitable_obj_header_t)
    fsm_obj_t;
    union {
        task_queue_t;
        task_queue_t tTaskQueue;
    };
END_DEF_STRUCTURE(fsm_waitable_obj_header_t)

DEF_STRUCTURE(fsm_event_t)
    fsm_waitable_obj_header_t;
    uint8_t             chEventFlag;    //!< signal
END_DEF_STRUCTURE(fsm_event_t)

DEF_STRUCTURE(fsm_mutex_t)
    fsm_waitable_obj_header_t;
    uint8_t             chMutexFlag;    //!< signal
END_DEF_STRUCTURE(fsm_mutex_t)

DEF_STRUCTURE(fsm_semaphore_t)
    fsm_waitable_obj_header_t;
    uint16_t            hwSemCounter;   //!< counter
    uint16_t            hwSemMaximum;   //!< Maximum counter 
END_DEF_STRUCTURE(fsm_semaphore_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __FSM_TYPES_H__
/* EOF */
