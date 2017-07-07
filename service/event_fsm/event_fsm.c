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
typedef uint8_t (efsm_state_t)(void *pEvent);
typedef efsm_state_t *efsm_stack_t;

typedef struct {
    efsm_stack_t   *Stack;
    uint8_t         StackSize;
    uint8_t         SP;           //!< stack point.
    uint8_t         CurrentSP;    //!< current stack point, it's alwayse less or equal to SP.
} efsm_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool efsm_init     (efsm_t         *EFSM,
                    efsm_stack_t   *pStack,
                    uint8_t         stackSize,
                    efsm_state_t   *pInitState)
{
    EFSM->Stack      = pStack;
    EFSM->StackSize  = stackSize;
    EFSM->SP         = 0;
    EFSM->CurrentSP  = 0;
    EFSM->Stack[0]   = pInitState;

    return true;
}

static bool efsm_current_level_decrease(efsm_t *EFSM)
{
    if (EFSM->CurrentSP != 0u) {
        EFSM->CurrentSP--;
        return true;
    } else {
        return false;
    }
}

//!< reset current-SP to SP.
static void efsm_reset_current(efsm_t *EFSM)
{
    EFSM->CurrentSP = EFSM->SP;
}

static efsm_state_t *efsm_get_current_state(efsm_t *EFSM)
{
    return EFSM->Stack[EFSM->CurrentSP];
}

bool efsm_level_to_current(efsm_t *EFSM)
{
    EFSM->SP = EFSM->CurrentSP;             //!< POP stack to current level.
    
    return true;
}

//! transfer to specified state within current level.
bool efsm_current_level_to_state(efsm_t *EFSM, efsm_state_t *pState)
{
    EFSM->Stack[EFSM->CurrentSP] = pState;   //!< transfer to State.

    return true;
}

//! transfer to specified state with level decrease.
//! This function is identically call efsm_transfer_to_current_level and
//! then call efsm_current_level_transfer_to.
bool efsm_to_state(efsm_t *EFSM, efsm_state_t *pState)
{
    EFSM->SP = EFSM->CurrentSP;             //!< POP stack to current level.
    EFSM->Stack[EFSM->SP] = pState;         //!< transfer to State.
    
    return true;
}

//! transfer to specified state that locate in a upper level.
bool efsm_to_uper(efsm_t *EFSM, efsm_state_t *pState)
{
    if ((EFSM->CurrentSP + 1u) >= EFSM->StackSize) { //!< avoid overflow.
        return false;
    }

    EFSM->CurrentSP++;
    EFSM->SP = EFSM->CurrentSP;             //!< POP stack to current level.
    EFSM->Stack[EFSM->SP] = pState;         //!< transfer to State.
    
    return true;
}

//! transfer to specified state that locate in a lower level.
bool efsm_to_lower(efsm_t *EFSM, efsm_state_t *pState)
{
    if (EFSM->CurrentSP == 0u) {            //!< avoid overflow.
        return false;
    }
    
    //! POP stack.
    EFSM->CurrentSP--;
    EFSM->SP = EFSM->CurrentSP;             //!< POP stack to current level.
    if (pState != NULL) {
        EFSM->Stack[EFSM->SP] = pState;     //!< transfer to specified state.
    } else {
                                            //!< no state transfer.
    }
    
    return true;
}


/*! \brief dispatch an event to an fsm.
 *  \param EFSM                 point to an event fsm.
 *  \param pEvent               point to an event. The type of event is base on application.
 *  \retval FSM_RT_CPL          event has been handle success.
 *  \retval FSM_RT_ONGOING      state has not been run complete.
 *  \retval FSM_RT_ERR          failed to get current state, might stack uninitialzed or voerflow.
 */
uint8_t efsm_dispatch_event(efsm_t *EFSM, void *pEvent)
{
    efsm_state_t *pState;
    uint8_t res;
    
__RUN_HANDLE:
    pState = efsm_get_current_state(EFSM);
    if (pState == NULL) {
        return FSM_RT_ERR;
    }
    res = (*pState)(pEvent);
    switch (res) {
        case FSM_RT_UNHANDLE:
            //! Try to handle this event at lower level if current state does not handle it.
            if (!efsm_current_level_decrease(EFSM)) {
                efsm_reset_current(EFSM);
                return FSM_RT_CPL;
            }
            goto __RUN_HANDLE;

        case FSM_RT_ONGOING:
            return FSM_RT_ONGOING;

        case FSM_RT_CPL:
        default:
            efsm_reset_current(EFSM);
            return FSM_RT_CPL;
    }
}


/* EOF */
