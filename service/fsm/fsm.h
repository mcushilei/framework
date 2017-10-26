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

#ifndef __FSM_H__
#define __FSM_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define FSM_TASK            fsm_tcb_t *
#define FSM_TASK_STACK      task_stack_t
#define FSM_OBJ             fsm_handle_t

#define FSM_INFINITE        (~0u)
                
#define FSM_EVENT_SINGNAL_BIT       (1u << 0)
#define FSM_EVENT_MANUAL_RESET_BIT  (1u << 1)

#define FSM_MUTEX_OWNED_BIT         (1u << 0)

/*============================ MACROFIED FUNCTIONS ===========================*/
//! \brief start define a FSM state
#define DEF_STATE(__NAME)       void __NAME(void *Arg)
#define REF_STATE(__NAME)       __NAME
#define PROTOTYPE_STATE(__NAME) void __NAME(void *Arg)

//! \brief state transfering
#define FSM_TRANSFER_TO(__ROUTINE, __ARG_ADDR)                                  \
            fsm_state_transfer(__ROUTINE, __ARG_ADDR)

//! \brief state complete
#define FSM_COMPLETE()                                                          \
            fsm_state_transfer(NULL, NULL)

//! \brief call sub sfm and return to current state when sub sfm run complete
#define FSM_CALL(__ROUTINE, __ARG_ADDR)                                         \
            fsm_call_sub(__ROUTINE, __ARG_ADDR)

/*============================ TYPES =========================================*/
enum {
    FSM_ERR_NONE            = 0,
    FSM_ERR_INVALID_PARAM,
    FSM_ERR_INVALID_OPT,
    FSM_ERR_OBJ_DEPLETED,
    FSM_ERR_OBJ_NOT_SINGLED,
    FSM_ERR_OBJ_NOT_WAITABLE,
    FSM_ERR_OBJ_TYPE,
    FSM_ERR_TASK_NO_MORE_TCB,
    FSM_ERR_TASK_PEND_TIMEOUT,
    FSM_ERR_TASK_STACK_FULL,
    FSM_ERR_PEND_ISR,
    FSM_ERR_CREATE_ISR,
    FSM_ERR_CALL_IN_ISR,
    FSM_ERR_SEM_EXCEED,
    FSM_ERR_NOT_MUTEX_OWNER,
};

enum {
    FSM_OBJ_TYPE_INVALID    = 0,
    FSM_OBJ_TYPE_TASK       = 0x01,
    FSM_OBJ_TYPE_WAITABLE   = 0x80,
    FSM_OBJ_TYPE_FLAG       = 0x80,
    FSM_OBJ_TYPE_MUTEX      = 0x81,
    FSM_OBJ_TYPE_SEM        = 0x82,
};

enum {
    FSM_TASK_STATUS_READY   = 0,
    FSM_TASK_STATUS_PEND,
    FSM_TASK_STATUS_PEND_OK,
    FSM_TASK_STATUS_PEND_TIMEOUT,
};

typedef uint8_t     fsm_err_t;
typedef void       *fsm_handle_t;
typedef void        fsm_state_t(void *);

typedef struct fsm_basis_obj_t      fsm_basis_obj_t;
typedef struct fsm_tcb_t            fsm_tcb_t;

typedef struct {
    fsm_state_t        *State;         //!< routine
    void               *Arg;           //!< argument
} task_stack_t;

struct fsm_tcb_t {
    fsm_tcb_t          *Next;
    fsm_tcb_t          *Prev;
    
    uint32_t            Delay;
    
    uint8_t             Flag;
    uint8_t             Status;

    uint8_t             StackSize;
    uint8_t             StackPoint;
    task_stack_t       *Stack;
    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    fsm_handle_t        Object;
    fsm_tcb_t          *WaitNodeNext;
    fsm_tcb_t          *WaitNodePrev;
#endif
};

struct fsm_basis_obj_t {
    union {
        struct {
            uint8_t         ObjType;
            uint8_t         ObjFlag;
        };
        fsm_basis_obj_t    *ObjNext;
    };
};

typedef struct {
    fsm_tcb_t           *Head;
    fsm_tcb_t           *Tail;
} fsm_task_list_t;

typedef struct {
    fsm_basis_obj_t;
    
    union {
        fsm_task_list_t;
        fsm_task_list_t     TaskQueue;
    };
} fsm_waitable_obj_t;

typedef struct {
    fsm_waitable_obj_t;
    uint8_t                 EventFlag;
} fsm_flag_t;

typedef struct {
    fsm_waitable_obj_t;
    fsm_tcb_t              *MutexOwner;
} fsm_mutex_t;

typedef struct {
    fsm_waitable_obj_t;
    uint16_t                SemCounter;
} fsm_semaphore_t;

typedef struct {
    fsm_tcb_t              *CurrentTask;
    fsm_task_list_t         ReadyList;
    fsm_task_list_t         PendList;
} scheduler_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern volatile uint8_t     fsmIntNesting;
extern scheduler_t          fsmScheduler;

/*============================ PROTOTYPES ====================================*/

/*----------------------------- miscellaneous --------------------------------*/
extern void         fsm_init            (void);
extern bool         fsm_scheduler       (void);

extern void         fsm_time_tick       (void);
extern fsm_err_t    fsm_time_delay      (uint32_t          wTimeout);

extern void         fsm_int_enter       (void);
extern void         fsm_int_exit        (void);

/*---------------------------- task management -------------------------------*/
extern fsm_err_t    fsm_task_create     (fsm_tcb_t        **pptTask,
                                         fsm_state_t       *State,
                                         void              *Arg,
                                         task_stack_t      *Stack,
                                         uint8_t            StackSize);
extern bool         fsm_state_transfer  (fsm_state_t       *State,
                                         void              *Arg);
extern fsm_err_t    fsm_call_sub        (fsm_state_t       *State,
                                         void              *Arg);


/*-------------------------------- flag event --------------------------------*/
extern fsm_err_t    fsm_flag_create        (fsm_handle_t       *pptEvent,
                                            bool                bManualReset,
                                            bool                bInitialState);
extern fsm_err_t    fsm_flag_wait          (fsm_handle_t        hObject,
                                            uint32_t            wTimeout);
extern fsm_err_t    fsm_flag_set           (fsm_handle_t        hObject);
extern fsm_err_t    fsm_flag_reset         (fsm_handle_t        hObject);

/*------------------------- mutual exclusion semaphore -----------------------*/
extern fsm_err_t    fsm_mutex_create       (fsm_handle_t       *pptMutex);
extern fsm_err_t    fsm_mutex_wait         (fsm_handle_t        hObject,
                                            uint32_t            wTimeout);
extern fsm_err_t    fsm_mutex_release      (fsm_handle_t        hObject);

/*-------------------------------- semaphore ---------------------------------*/
extern fsm_err_t    fsm_semaphore_create   (fsm_handle_t       *pptSem,
                                            uint16_t            hwInitialCount);
extern fsm_err_t    fsm_semaphore_wait     (fsm_handle_t        hObject,
                                            uint32_t            wTimeout);
extern fsm_err_t    fsm_semaphore_release  (fsm_handle_t        hObject,
                                            uint16_t            hwReleaseCount);

/*--------------------------- internal use only ------------------------------*/
extern void         fsm_set_task_ready         (fsm_tcb_t          *pTask,
                                                uint8_t             pendStat);
extern void         fsm_set_task_pend          (uint32_t            timeDelay);

extern void         fsm_waitable_obj_add_task  (fsm_waitable_obj_t *pObj,
                                                fsm_tcb_t          *pTask);
extern void         fsm_waitable_obj_remove_task(fsm_waitable_obj_t *pObj,
                                                fsm_tcb_t          *pTask);
extern fsm_tcb_t   *fsm_waitable_obj_get_task  (fsm_waitable_obj_t *pObj);


#endif  //! #ifndef __FSM_H__
/* EOF */
