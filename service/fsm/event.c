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




/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm.h"

#if SAFE_TASK_THREAD_SYNC == ENABLED

/*============================ MACROS ========================================*/
#if ((!defined(FSM_MAX_EVENTS)) || (FSM_MAX_EVENTS < 1))
#   define FSM_MAX_EVENTS           (1u)
#   warning "FSM_MAX_EVENTS is invalid, use default value 1u."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static fsm_event_t   *sptEventList;              //! Head of event ocb pool
static fsm_event_t    stEventPool[FSM_MAX_EVENTS];       //! event ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_event_init(void)
{
    uint_fast8_t n;
    fsm_event_t *p = stEventPool;

    MEM_SET_ZERO((void *)stEventPool, sizeof(stEventPool));
    sptEventList = NULL;
    
    //! add events ocb to the free list
    for (n = UBOUND(stEventPool); n; n--) {
        p->ptObjNext = (fsm_obj_t *)sptEventList;    //! add task item to freelist
        p->chObjType = FSM_OBJ_TYPE_EVENT;
        sptEventList = p;
        p++;
    }
}

/*! \brief initialize task event
 *! \param ptEvent event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
uint_fast8_t fsm_event_create(fsm_event_t **pptEvent, bool bManualReset, bool bInitialState)
{
    uint8_t chSignal;
    fsm_event_t *ptEvent;
    
    if (NULL == pptEvent) {
        return FSM_ERR_NULL_PTR;
    }

    //!< get OCB from pool.
    if (NULL == sptEventList) {
        *pptEvent = NULL;
        return FSM_ERR_OBJ_NO_MORE_OCB;
    } else {
        //! register object here.
        *pptEvent = sptEventList;

        ptEvent = sptEventList;
        sptEventList = (fsm_event_t *)ptEvent->ptObjNext;
    }
    
    chSignal = 0;
    if (bManualReset) {
        chSignal |= FSM_SIGNAL_MANUAL_RESET_BIT;
    }
    if (bInitialState) {
        chSignal |= FSM_SIGNAL_FLAG_BIT;
    }
    
    SAFE_ATOM_CODE(
        ptEvent->chObjType     = FSM_OBJ_TYPE_EVENT;
        ptEvent->ptObjNext     = NULL;
        ptEvent->ptTCBHead     = NULL;           
        ptEvent->ptTCBTail     = NULL;
        ptEvent->chSignal      = chSignal;  //!< set initial state
    )

    return FSM_ERR_NONE;
}

/*! \brief set task event
 *! \param ptEvent pointer for task event
 *! \return none
 */
void fsm_event_set(fsm_event_t *ptEvent) 
{
    if (NULL == ptEvent) {
        return;
    }
    
    SAFE_ATOM_CODE(
        fsm_tcb_t *pTask;
        
        ptEvent->chSignal |= FSM_SIGNAL_FLAG_BIT;
        
        //! wake up blocked tasks
        for (pTask = ptEvent->ptTCBHead; NULL != pTask; pTask = pTask->pNext) {
            pTask->ptObject = NULL;
            fsm_set_task_ready(pTask);    //!< move task to ready table.
        }
        ptEvent->ptTCBHead = NULL;    //!< clear waiting tasks list
        ptEvent->ptTCBTail = NULL;
    )
}

/*! \brief reset specified task event
 *! \param ptEvent task event pointer
 *! \return none
 */
void fsm_event_reset(fsm_event_t *ptEvent)
{
    if (NULL == ptEvent) {
        return;
    }

    SAFE_ATOM_CODE(
        ptEvent->chSignal &= ~FSM_SIGNAL_FLAG_BIT;
    )
}
#endif

/* EOF */
