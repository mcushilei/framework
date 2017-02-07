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
/**
 * all the action excuated on transfer should be implemented explicitly by user.
**/

#ifndef __EVENT_FSM_C__
#ifndef __EVENT_FSM_H__
#define __EVENT_FSM_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef uint8_t (fn_event_state_t)(void *pArg);
typedef fn_event_state_t *event_fsm_stack_t;

typedef struct {
    event_fsm_stack_t   *Stack;
    uint8_t             StackSize;
    uint8_t             SP;           //!< stack point.
    uint8_t             CurrentSP;    //!< current stack point, it's alwayse less or equal to SP.
} event_fsm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool     event_fsm_init(event_fsm_t          *EFSM,
                               event_fsm_stack_t    *Stack,
                               uint8_t              StackSize,
                               fn_event_state_t     *InitState);
extern uint8_t  event_fsm_dispatch_event(event_fsm_t *EFSM, void *Event);

extern bool event_fsm_transfer_to_uper(event_fsm_t *EFSM, fn_event_state_t *State);
extern bool event_fsm_transfer_to_lower(event_fsm_t *EFSM, fn_event_state_t *State);
extern bool event_fsm_transfer_to_current(event_fsm_t *EFSM);
extern bool event_fsm_current_transfer_to(event_fsm_t *EFSM, fn_event_state_t *State);
extern bool event_fsm_transfer_to(event_fsm_t *EFSM, fn_event_state_t *State);


#endif  //! #ifndef __EVENT_FSM_H__
#endif  //! #ifndef __EVENT_FSM_C__
/* EOF */
