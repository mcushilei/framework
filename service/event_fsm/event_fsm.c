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
    event_fsm_stack_t *pStack;
    uint8_t     chStackSize;
    uint8_t     chSP;           //!< stack point.
    uint8_t     chCurrentSP;    //!< current stack point, it's alwayse less or equal to chSP.
} event_fsm_tcb_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool event_fsm_init(event_fsm_tcb_t *ptTCB,
                    event_fsm_stack_t *pStack,
                    uint8_t chStackSize,
                    fn_event_state_t *pInitState)
{
    ptTCB->pStack       = pStack;
    ptTCB->chStackSize  = chStackSize;
    ptTCB->chSP         = 0;
    ptTCB->chCurrentSP  = 0;
    ptTCB->pStack[0]    = pInitState;

    return true;
}

//! internal use only.
static bool event_fsm_current_level_decrease(event_fsm_tcb_t *ptTCB)
{
    if (ptTCB->chCurrentSP) {
        ptTCB->chCurrentSP--;
        return true;
    } else {
        return false;
    }
}

static void event_fsm_reset_current(event_fsm_tcb_t *ptTCB)
{
    ptTCB->chCurrentSP = ptTCB->chSP;       //!< reset current SP.
}

static fn_event_state_t *event_fsm_get_current_state(event_fsm_tcb_t *ptTCB)
{
    return ptTCB->pStack[ptTCB->chCurrentSP];
}

bool event_fsm_transfer_to_current(event_fsm_tcb_t *ptTCB)
{
    ptTCB->chSP = ptTCB->chCurrentSP;
    return true;
}

//! transfer to specified state that locate in the same level(This is called in current level obviously).
bool event_fsm_transfer_to(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    if (pState == NULL) {
        return false;
    }
    
    ptTCB->chSP = ptTCB->chCurrentSP;
    ptTCB->pStack[ptTCB->chSP] = pState;
    
    return true;
}

//! transfer to specified state that locate in a upper level.
bool event_fsm_transfer_to_uper(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    if ((ptTCB->chCurrentSP + 1) == ptTCB->chStackSize) {  //!< avoid overflow.
        return false;
    }

    if (pState == NULL) {
        return false;
    }
    
    ptTCB->chCurrentSP++;
    ptTCB->chSP = ptTCB->chCurrentSP;
    ptTCB->pStack[ptTCB->chSP] = pState;
    
    return true;
}

//! transfer to specified state that locate in a lower level.
bool event_fsm_transfer_to_lower(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    if (ptTCB->chCurrentSP == 0) {
        return false;
    }
    
    ptTCB->chCurrentSP--;
    ptTCB->chSP = ptTCB->chCurrentSP;
    if (pState != NULL) {
        ptTCB->pStack[ptTCB->chSP] = pState;
    }
    
    return true;
}


fsm_rt_t event_fsm_dispatch_event(event_fsm_tcb_t *ptTCB, void *ptEvent)
{
    fn_event_state_t *fnCurrentState;
    uint8_t chRes;
    
    do {
        fnCurrentState = event_fsm_get_current_state(ptTCB);
        if (NULL == fnCurrentState) {
            return FSM_RT_ERR;
        }
        chRes = (*fnCurrentState)(ptEvent);
        if (FSM_RT_UNHANDLE == chRes) {
            if (!event_fsm_current_level_decrease(ptTCB)) {
                event_fsm_reset_current(ptTCB);
                return FSM_RT_CPL;
            }
        } else if (FSM_RT_ONGOING == chRes) {
            return FSM_RT_ONGOING;
        } else {
            event_fsm_reset_current(ptTCB);
            return FSM_RT_CPL;
        }
    } while (1);
}


/* EOF */
