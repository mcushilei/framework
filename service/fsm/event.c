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
#   warning "FSM_MAX_EVENTS is invalid, use default value 1."
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
    fsm_event_t **p;

    MEM_SET_ZERO((void *)stEventPool, sizeof(stEventPool));
    p = &sptEventList;
    
    //! add event OCBs to the free list
    for (n = 0; n < ARRAY_LENGTH(stEventPool); n++) {
        *p = &stEventPool[n];
        p = (fsm_event_t **)&((*p)->ObjNext);
    }
}

/*! \brief initialize task event
 *! \param ptEvent event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
uint_fast8_t fsm_event_create(
          fsm_event_t **pptEvent,
          bool bManualReset,
          bool bInitialState)
{
    uint8_t Flag;
    fsm_event_t *ptEvent;
    
    if (NULL == pptEvent) {
        return FSM_ERR_INVALID_PARAM;
    }

    //!< get OCB from pool.
    if (NULL == sptEventList) {
        *pptEvent = NULL;
        return FSM_ERR_OBJ_NO_MORE_OCB;
    }
    
    ptEvent      = sptEventList;
    sptEventList = (fsm_event_t *)ptEvent->ObjNext;
    
    Flag = 0;
    if (bManualReset) {
        Flag |= FSM_EVENT_MANUAL_RESET_BIT;
    }
    if (bInitialState) {
        Flag |= FSM_EVENT_SINGNAL_BIT;
    }
    
    SAFE_ATOM_CODE(
        ptEvent->ObjType      = FSM_OBJ_TYPE_EVENT;
        ptEvent->ObjNext      = NULL;
        ptEvent->Head      = NULL;           
        ptEvent->Tail      = NULL;
        ptEvent->EventFlag    = Flag;   //!< set initial state
        fsm_register_object(ptEvent);       //!< register object.
    )
    *pptEvent = ptEvent;

    return FSM_ERR_NONE;
}

/*! \brief set task event
 *! \param ptEvent pointer for task event
 *! \return none
 */
uint_fast8_t fsm_event_set(fsm_event_t *ptEvent) 
{
    if (NULL == ptEvent) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (ptEvent->ObjType != FSM_OBJ_TYPE_EVENT) {
        return FSM_ERR_OBJ_TYPE_MISMATCHED;
    }
    
    SAFE_ATOM_CODE(
        fsm_tcb_t *pTask, *pNextTask;
        
        ptEvent->EventFlag |= FSM_EVENT_SINGNAL_BIT;
        if (ptEvent->Head != NULL) {
            //! wake up all blocked tasks.
            for (pTask = ptEvent->Head; NULL != pTask; pTask = pNextTask) {
                pNextTask = pTask->Next;
                fsm_set_task_ready(pTask);    //!< move task to ready list.
                pTask->Object = NULL;
                pTask->Status = FSM_TASK_STATUS_PEND_OK;
            }
            ptEvent->Head = NULL;
            ptEvent->Tail = NULL;
            if (!(ptEvent->EventFlag & FSM_EVENT_MANUAL_RESET_BIT)) {
                ptEvent->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
            }
        }
    )
        
    return FSM_ERR_NONE;
}

/*! \brief reset specified task event
 *! \param ptEvent task event pointer
 *! \return none
 */
uint_fast8_t fsm_event_reset(fsm_event_t *ptEvent)
{
    if (NULL == ptEvent) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (ptEvent->ObjType != FSM_OBJ_TYPE_EVENT) {
        return FSM_ERR_OBJ_TYPE_MISMATCHED;
    }
    
    if (!(ptEvent->EventFlag & FSM_EVENT_MANUAL_RESET_BIT)) {
        return FSM_ERR_OPT_NOT_SUPPORT;
    }

    SAFE_ATOM_CODE(
        ptEvent->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
    )
        
    return FSM_ERR_NONE;
}
#endif

/* EOF */
