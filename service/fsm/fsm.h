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
#define FSM_TASK            fsm_tcb_t *
#define FSM_TASK_STACK      task_stack_t
#define FSM_OBJ_EVENT       fsm_flag_t *
#define FSM_OBJ_MUTEX       fsm_mutex_t *
#define FSM_OBJ_SEMAPHORE   fsm_semaphore_t *
#define FSM_OBJ             void *
                
/*============================ MACROFIED FUNCTIONS ===========================*/
#define FSM_SCHEDULER()         fsm_scheduler()
#define FSM_INIT()              fsm_init()

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

//! \brief call sub sfm and return to specified state when sub sfm run complete
#define FSM_CALL_EX(__ROUTINE, __ARG_ADDR, __RET_ROUTINE, __RET_ARG_ADDR)       \
            fsm_call_sub_ex(__ROUTINE, __ARG_ADDR,                \
                            __RET_ROUTINE, __RET_ARG_ADDR)

                
#define FSM_INT_ENTER()     do {\
                if (gchFSMIntNesting < 255u) {\
                    gchFSMIntNesting++;\
                }\
            while (0)
                
#define FSM_INT_EXIT()     do {\
                if (gchFSMIntNesting) {\
                    gchFSMIntNesting--;\
                }\
            while (0)
                
#define FSM_IN_INT()    (0 != gchFSMIntNesting)
                
#if SAFE_TASK_THREAD_SYNC == ENABLED
#define FSM_WAIT_SINGLE_OBJECT(__EVENT)                  \
            fsm_wait_for_single_object(__EVENT, 0)
#endif      //! #if SAFE_TASK_THREAD_SYNC == ENABLED


//! \brief initialize a task flag item
#define FSM_CREATE_EVENT(__PEVENT, __MANUAL_RESET, __INITIAL_STATE)    \
            fsm_flag_create(&(__PEVENT),(__MANUAL_RESET), (__INITIAL_STATE))

//! \brief set task flag to active state
#define FSM_SET_EVENT(__EVENT)                   \
            fsm_flag_set((__EVENT))

//! \brief reset task flag to inactive state
#define FSM_RESET_EVENT(__EVENT)                 \
            fsm_flag_reset((__EVENT))

#define FSM_CREATE_MUTEX(__PMUTEX)\
            fsm_mutex_create(&(__PMUTEX))
                
#define FSM_RELEASE_MUTEX(__PMUTEX)                \
            fsm_mutex_release(__PMUTEX)
                
#define FSM_CREATE_SEM(__PSEM, __INIT_COUNT, __MAX_COUNT)        \
            fsm_semaphore_create(&(__PSEM), __INIT_COUNT, __MAX_COUNT)

#define FSM_RELEASE_SEM(__PSEM, __RELEASE_COUNT)                \
            fsm_semaphore_release(__PSEM, __RELEASE_COUNT)
                
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern volatile uint8_t gchFSMIntNesting;

/*============================ PROTOTYPES ====================================*/
extern void         fsm_init            (void);

extern uint_fast8_t fsm_task_create     (
                                        fsm_tcb_t **        pptTask,
                                        state_func_t *      State,
                                        void *              Arg,
                                        task_stack_t *      Stack,
                                        uint_fast8_t        StackSize);
extern bool         fsm_state_transfer  (
                                        state_func_t *      State,
                                        void *              Arg);
extern uint_fast8_t fsm_call_sub_ex     (
                                        state_func_t *      State,
                                        void *              Arg,
                                        state_func_t *      ReturnState,
                                        void *              ReturnArg);
extern uint_fast8_t fsm_call_sub        (
                                        state_func_t *      State,
                                        void *              Arg);

extern bool         fsm_scheduler       (void);
extern void         fsm_time_tick       (void);

#if SAFE_TASK_THREAD_SYNC == ENABLED
extern uint_fast8_t fsm_wait_for_single_object(void *Object, uint32_t wTimeout);

extern uint_fast8_t fsm_flag_create        (
                                            fsm_flag_t **      pptEvent,
                                            bool                bManualReset,
                                            bool                bInitialState);
extern uint_fast8_t fsm_flag_set           (fsm_flag_t *      ptEvent);
extern uint_fast8_t fsm_flag_reset         (fsm_flag_t *      ptEvent);
extern uint_fast8_t fsm_mutex_create        (fsm_mutex_t **     pptMutex);
extern uint_fast8_t fsm_mutex_release       (fsm_mutex_t *      ptMutex);
extern uint_fast8_t fsm_semaphore_create    (
                                            fsm_semaphore_t **  pptSem,
                                            uint16_t            hwInitialCount,
                                            uint16_t            hwMaximumCount);
extern uint_fast8_t fsm_semaphore_release   (fsm_semaphore_t *  ptSem,
                                            uint16_t            hwReleaseCount);
#endif      //! #if SAFE_TASK_THREAD_SYNC == ENABLED

/*--------------------------- Internal use only ------------------------------*/
extern bool         fsm_task_enqueue            (
                                                task_queue_t *      pTaskQueue,
                                                fsm_tcb_t *         pTask);
extern fsm_tcb_t *  fsm_task_dequeue            (task_queue_t *     pTaskQueue);
extern bool         fsm_remove_task_from_queue  (
                                                task_queue_t *      pTaskQueue,
                                                fsm_tcb_t *         pTask);
extern bool         fsm_set_task_ready          (fsm_tcb_t *        pTask);
extern bool         fsm_register_object         (void *             Obj);
extern bool         fsm_deregister_object       (void *             Obj);


#endif  //! #ifndef __FSM_H__
/* EOF */
