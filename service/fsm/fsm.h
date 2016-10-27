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
#include ".\fsm_types.h"

/*============================ MACROS ========================================*/
#define FSM_START_STATE         0

/*============================ MACROFIED FUNCTIONS ===========================*/
#define FSM_SCHEDULER()         fsm_scheduler()
#define FSM_INIT()              fsm_init()

//! \brief start define a FSM state
#define DEF_STATE(__NAME)       void __NAME(void *pArg)
#define REF_STATE(__NAME)       __NAME
#define PROTOTYPE_STATE(__NAME) void __NAME(void *pArg)

//! \brief state transfering
#define FSM_TRANSFER_TO(__ROUTINE, __ARG_ADDR)                                  \
            fsm_state_transfer(__ROUTINE, __ARG_ADDR)

//! \brief state complete
#define FSM_COMPLETE()                                                          \
            fsm_state_transfer(NULL, NULL)

//! \brief call sub state machine and return to current state when sub state machine run complete
#define FSM_CALL(__ROUTINE, __ARG_ADDR)                                         \
            fsm_call_sub(__ROUTINE, __ARG_ADDR)

//! \brief call sub state machine and return to specified state when sub state machine run complete
#define FSM_CALL_EX(__ROUTINE, __ARG_ADDR, __RET_ROUTINE, __RET_ARG_ADDR)       \
            fsm_call_sub_ex(__ROUTINE, __ARG_ADDR,                \
                            __RET_ROUTINE, __RET_ARG_ADDR)

#if SAFE_TASK_THREAD_SYNC == ENABLED
#define FSM_WAIT_SINGLE_OBJECT(__EVENT)                  \
            fsm_wait_for_single_object((__EVENT))
#endif      //! #if SAFE_TASK_THREAD_SYNC == ENABLED


//! \brief initialize a task event item
#define FSM_CREATE_EVENT(__PEVENT, __MANUAL_RESET, __INITIAL_STATE)    \
            fsm_event_create(&(__PEVENT),(__MANUAL_RESET), (__INITIAL_STATE))

//! \brief set task event to active state
#define FSM_SET_EVENT(__EVENT)                   \
            fsm_event_set((__EVENT))

//! \brief reset task event to inactive state
#define FSM_RESET_EVENT(__EVENT)                 \
            fsm_event_reset((__EVENT))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern void         fsm_init            (void);
extern void         fsm_deinit          (void);
extern uint_fast8_t fsm_task_create(
                                        fsm_tcb_t **        pptTask,
                                        state_func_t *      fnState,
                                        void *              pArg,
                                        task_stack_item_t * pStack,
                                        uint_fast8_t        chStackSize);
extern bool         fsm_state_transfer  (
                                        state_func_t *      fnState,
                                        void *              pArg);
extern bool         fsm_call_sub_ex     (
                                        state_func_t *      fnState,
                                        void *              pArg,
                                        state_func_t *      fnReturnRoutine,
                                        void *              pReturnArg);
extern bool         fsm_call_sub        (
                                        state_func_t *      fnState,
                                        void *              pArg);

extern bool         fsm_set_task_ready  (fsm_tcb_t *pTask);
extern bool         fsm_scheduler       (void);

#if SAFE_TASK_THREAD_SYNC == ENABLED
extern uint_fast8_t     fsm_wait_for_single_object(void *ptObject);
#endif      //! #if SAFE_TASK_THREAD_SYNC == ENABLED

extern uint_fast8_t     fsm_event_create    (
                                            fsm_event_t **      pptEvent,
                                            bool                bManualReset,
                                            bool                bInitialState);
extern void             fsm_event_set      (fsm_event_t *       ptEvent);
extern void             fsm_event_reset    (fsm_event_t *       ptEvent);


#endif  //! #ifndef __FSM_H__
/* EOF */
