/*******************************************************************************
 *  Copyright(C)2016-2018 by Dreistein<mcu_shilei@hotmail.com>                *
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
#include ".\event_fsm_public.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool efsm_init     (efsm_t         *EFSM,
                    efsm_stack_t   *pStack,
                    uint8_t         stackSize,
                    efsm_state_t   *pInitState)
{
    EFSM->Stack         = pStack;
    EFSM->StackSize     = stackSize;
    EFSM->StackLevel    = 0;
    EFSM->CurrentLevel  = 0;
    EFSM->Stack[0]      = pInitState;
    EFSM->Status        = EFSM_STATUS_STOPPED;

    return true;
}

static bool efsm_current_level_decrease(efsm_t *EFSM)
{
    if (EFSM->CurrentLevel != 0u) {
        EFSM->CurrentLevel--;
        return true;
    } else {
        return false;
    }
}

//!< reset current-StackLevel to StackLevel.
static void efsm_reset_current_level(efsm_t *EFSM)
{
    EFSM->CurrentLevel = EFSM->StackLevel;
}

static efsm_state_t *efsm_get_current_state(efsm_t *EFSM)
{
    return EFSM->Stack[EFSM->CurrentLevel];
}

//! to add layer to current layer. this will destroy all layers over current layer at first and then add the new one.
bool efsm_to_upper(efsm_t *EFSM, efsm_state_t *pState)
{
    if ((EFSM->CurrentLevel + 1u) >= EFSM->StackSize) { //!< avoid overflow.
        return false;                                   //!< this should be fatal error!
    }

    if (pState == NULL) {
        return false;
    }

    EFSM->Status = EFSM_STATUS_ADD_LAYER;
    EFSM->CurrentLevel++;
    EFSM->StackLevel = EFSM->CurrentLevel;      //!< reset to current layer.
    EFSM->Stack[EFSM->CurrentLevel] = pState;   //!< transfer to State.

    return true;
}

//! to destroy all layers over current layer.
bool efsm_to_current(efsm_t *EFSM)
{
    EFSM->StackLevel = EFSM->CurrentLevel;
    
    return true;
}

//! to reduce current layer. note this will destroy current layer and all those over it if exist.
bool efsm_to_lower(efsm_t *EFSM)
{
    if (EFSM->CurrentLevel == 0u) {             //!< avoid overflow.
        return false;
    }
    
    //! POP stack.
    EFSM->CurrentLevel--;
    EFSM->StackLevel = EFSM->CurrentLevel;      //!< POP stack to current level.
    
    return true;
}

//! to take a state transfer on current layer. note this will have NO effect on level of layer.
bool efsm_current_layer_to_state(efsm_t *EFSM, efsm_state_t *pState)
{
    EFSM->Status = EFSM_STATUS_TRANSFER_STATE;
    EFSM->Stack[EFSM->CurrentLevel] = pState;       //!< transfer to State.

    return true;
}

//! to take a state transfer on current layer. note this will DESTROY ALL layers over current layer.
bool efsm_to_state(efsm_t *EFSM, efsm_state_t *pState)
{
    if (EFSM->StackLevel == EFSM->CurrentLevel) {
        EFSM->Status = EFSM_STATUS_TRANSFER_STATE;
    }
    EFSM->StackLevel = EFSM->CurrentLevel;          //!< delete all layers above current layer.
    EFSM->Stack[EFSM->CurrentLevel] = pState;       //!< transfer to state.

    return true;
}

/*! \brief dispatch an event to an fsm.
 *  \param EFSM                 point to an event fsm.
 *  \param pEvent               point to an event. The type of event is base on application.
 *  \retval FSM_RT_CPL          event has been handle success.
 *  \retval FSM_RT_ONGOING      state has not been run complete.
 *  \retval FSM_RT_ERR          failed to get current state, might stack uninitialzed or voerflow.
 */
uint8_t efsm_dispatch_event(efsm_t *EFSM, event_code_t event, void *arg)
{
    efsm_state_t *pState;
    uint8_t res;
    
    pState = efsm_get_current_state(EFSM);
    if (pState == NULL) {
        return FSM_RT_ERR;
    }
    switch (EFSM->Status) {
        case EFSM_STATUS_STOPPED:
        case EFSM_STATUS_TRANSFER_STATE:
        case EFSM_STATUS_ADD_LAYER:
            (*pState)(EFSM_EVENT_ENTRY_STATE, NULL);
            break;

        default:
            break;
    }
    EFSM->Status = EFSM_STATUS_RUNNING;
    res = (*pState)(event, arg);        //!< run this handler.
    switch (EFSM->Status) {
        case EFSM_STATUS_TRANSFER_STATE:
            (*pState)(EFSM_EVENT_EXIT_STATE, NULL);
            break;

        default:
            break;
    }
    switch (res) {
        case FSM_RT_UNHANDLE:
            //! Try to handle this event at lower level if current state does not handle it.
            if (!efsm_current_level_decrease(EFSM)) {
                efsm_reset_current_level(EFSM);
                return FSM_RT_CPL;
            }
            return FSM_RT_ONGOING;

        case FSM_RT_ONGOING:
            return FSM_RT_ONGOING;

        case FSM_RT_CPL:
        default:
            efsm_reset_current_level(EFSM);
            return FSM_RT_CPL;
    }
}


/* EOF */
