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

bool event_fsm_to_current_Level(event_fsm_tcb_t *ptTCB)
{
    ptTCB->chSP = ptTCB->chCurrentSP;
    return true;
}

//! transfer to specified state that locate in the same level(This is called in current level obviously).
bool event_fsm_transfer_to(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    ptTCB->chSP = ptTCB->chCurrentSP;
    ptTCB->pStack[ptTCB->chSP] = pState;
    
    return true;
}

//! transfer to specified state that locate in a upper level.
bool event_fsm_transfer_to_uper(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    if ((ptTCB->chSP + 1) == ptTCB->chStackSize) {  //!< avoid overflow.
        return false;
    }

    ptTCB->chSP++;                          //!< push stack.
    ptTCB->pStack[ptTCB->chSP] = pState;
    ptTCB->chCurrentSP = ptTCB->chSP;       //!< reset current SP.
    
    return true;
}

//! internal use only.
bool event_fsm_current_level_decrease(event_fsm_tcb_t *ptTCB)
{
    if (ptTCB->chCurrentSP) {
        ptTCB->chCurrentSP--;
        return true;
    } else {
        return false;
    }
}

//! transfer to specified state that locate in a lower level.
bool event_fsm_transfer_to_lower(event_fsm_tcb_t *ptTCB, fn_event_state_t *pState)
{
    if (!event_fsm_current_level_decrease(ptTCB)) {   //!< avoid downflow.
        return false;
    }

    return event_fsm_transfer_to(ptTCB, pState);
}

//! current level has been run complete.
bool event_fsm_transfer_to_end(event_fsm_tcb_t *ptTCB)
{
    ptTCB->pStack[ptTCB->chCurrentSP] = NULL;
    event_fsm_current_level_decrease(ptTCB);
    ptTCB->chSP = ptTCB->chCurrentSP;
    
    return true;
}

fn_event_state_t *event_fsm_get_current_state(event_fsm_tcb_t *ptTCB)
{
    return ptTCB->pStack[ptTCB->chCurrentSP];
}

fn_event_state_t *event_fsm_get_state(event_fsm_tcb_t *ptTCB)
{
    ptTCB->chCurrentSP = ptTCB->chSP;       //!< reset current SP.
    return ptTCB->pStack[ptTCB->chSP];
}

fsm_rt_t event_fsm_handle_event(event_fsm_tcb_t *ptFSM, void *ptEvent)
{
    static enum {
        EVENT_HANDLE_START      = 0,
        EVENT_HANDLE_RUN_HANDLE,
    } s_tState = EVENT_HANDLE_START;
    static fn_event_state_t *fnCurrentState;
    
    switch (s_tState) {
        case EVENT_HANDLE_START:
            fnCurrentState = event_fsm_get_state(ptFSM);
            if (NULL == fnCurrentState) {
                return fsm_rt_cpl;
            }
            s_tState = EVENT_HANDLE_RUN_HANDLE;
        //break;
            
        case EVENT_HANDLE_RUN_HANDLE:
        {
            uint8_t chRes;
            chRes = (*fnCurrentState)(ptEvent);
            if (fsm_rt_unhandle == chRes) {
                if (!event_fsm_current_level_decrease(ptFSM)) {
                    s_tState = EVENT_HANDLE_START;
                    return fsm_rt_cpl;
                } else {
                    fnCurrentState = event_fsm_get_current_state(ptFSM);
                    if (NULL == fnCurrentState) {
                        s_tState = EVENT_HANDLE_START;
                        return fsm_rt_cpl;
                    }
                }
            } else if (fsm_rt_ongoing == chRes) {
                break;
            } else {
                s_tState = EVENT_HANDLE_START;
                return fsm_rt_cpl;
            }
        }
        break;
        
        default:
        break;
    }
    
    return fsm_rt_ongoing;
}


/* EOF */
