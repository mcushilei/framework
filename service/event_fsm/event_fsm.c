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

//! \note do not move this pre-processor statement to other places
#define __EVENT_FSM_C__

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

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool event_fsm_init(event_fsm_t         *EFSM,
                    event_fsm_stack_t   *Stack,
                    uint8_t             StackSize,
                    fn_event_state_t    *InitState)
{
    EFSM->Stack       = Stack;
    EFSM->StackSize  = StackSize;
    EFSM->SP         = 0;
    EFSM->CurrentSP  = 0;
    EFSM->Stack[0]    = InitState;

    return true;
}

//! internal use only.
static bool event_fsm_current_level_decrease(event_fsm_t *EFSM)
{
    if (EFSM->CurrentSP) {
        EFSM->CurrentSP--;
        return true;
    } else {
        return false;
    }
}

static void event_fsm_reset_current(event_fsm_t *EFSM)
{
    EFSM->CurrentSP = EFSM->SP;       //!< reset current SP.
}

static fn_event_state_t *event_fsm_get_current_state(event_fsm_t *EFSM)
{
    return EFSM->Stack[EFSM->CurrentSP];
}

bool event_fsm_transfer_to_current(event_fsm_t *EFSM)
{
    EFSM->SP = EFSM->CurrentSP;
    return true;
}

//! transfer to specified state that locate in the same level(This is called in current level obviously).
bool event_fsm_transfer_to(event_fsm_t *EFSM, fn_event_state_t *State)
{
    if (State == NULL) {
        return false;
    }
    
    EFSM->SP = EFSM->CurrentSP;
    EFSM->Stack[EFSM->SP] = State;
    
    return true;
}

//! transfer to specified state that locate in a upper level.
bool event_fsm_transfer_to_uper(event_fsm_t *EFSM, fn_event_state_t *State)
{
    if ((EFSM->CurrentSP + 1) == EFSM->StackSize) {  //!< avoid overflow.
        return false;
    }

    if (State == NULL) {
        return false;
    }
    
    EFSM->CurrentSP++;
    EFSM->SP = EFSM->CurrentSP;
    EFSM->Stack[EFSM->SP] = State;
    
    return true;
}

//! transfer to specified state that locate in a lower level.
bool event_fsm_transfer_to_lower(event_fsm_t *EFSM, fn_event_state_t *State)
{
    if (EFSM->CurrentSP == 0) {
        return false;
    }
    
    EFSM->CurrentSP--;
    EFSM->SP = EFSM->CurrentSP;
    if (State != NULL) {
        EFSM->Stack[EFSM->SP] = State;
    }
    
    return true;
}


fsm_rt_t event_fsm_dispatch_event(event_fsm_t *EFSM, void *Event)
{
    fn_event_state_t *fnCurrentState;
    uint8_t chRes;
    
    do {
        fnCurrentState = event_fsm_get_current_state(EFSM);
        if (NULL == fnCurrentState) {
            return FSM_RT_ERR;
        }
        chRes = (*fnCurrentState)(Event);
        if (FSM_RT_UNHANDLE == chRes) {
            if (!event_fsm_current_level_decrease(EFSM)) {
                event_fsm_reset_current(EFSM);
                return FSM_RT_CPL;
            }
        } else if (FSM_RT_ONGOING == chRes) {
            return FSM_RT_ONGOING;
        } else {
            event_fsm_reset_current(EFSM);
            return FSM_RT_CPL;
        }
    } while (1);
}


/* EOF */
