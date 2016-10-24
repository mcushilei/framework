/*******************************************************************************
 *  Copyright(C)2015 by Dreistein<mcu_shilei@hotmail.com>                     *
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
#define __FSM_EVENT_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm_types.h"
#include ".\scheduler.h"

#if SAFE_TASK_THREAD_SYNC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief initialize task event
 *! \param ptEvent event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
event_t *create_event(event_t *ptEvent, bool bManualReset, bool bInitialState)
{
    if (NULL == ptEvent) {
        return NULL;
    }

    SAFE_ATOM_CODE(
        ptEvent->bSignal        = bInitialState;  //!< set initial state
        ptEvent->bManualReset   = bManualReset;   //!< manual reset flag
        ptEvent->ptHead         = NULL;           
        ptEvent->ptTail         = NULL;
    )

    return (event_t *)ptEvent;
}

/*! \brief set task event
 *! \param ptEvent pointer for task event
 *! \return none
 */
void set_event(event_t *ptEvent) 
{
    if (NULL == ptEvent) {
        return;
    }
    
    SAFE_ATOM_CODE(
        fsm_tcb_t *pTask;
        
        ptEvent->bSignal = true;
        
        //! wake up blocked tasks
        for (pTask = ptEvent->ptHead; NULL != pTask; pTask = pTask->pNext) {
            pTask->pObject = NULL;
            scheduler_register_task(pTask);    //!< move task to ready table.
        }
        ptEvent->ptHead = NULL;    //!< clear waiting tasks list
        ptEvent->ptTail = NULL;
    )
}

/*! \brief reset specified task event
 *! \param ptEvent task event pointer
 *! \return none
 */
void reset_event(event_t *ptEvent)
{
    if (NULL == ptEvent) {
        return;
    }

    SAFE_ATOM_CODE(
        ptEvent->bSignal = false;
    )
}
#endif

/* EOF */
