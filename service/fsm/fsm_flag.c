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
#if ((!defined(FSM_MAX_FLAGS)) || (FSM_MAX_FLAGS < 1))
#   define FSM_MAX_FLAGS           (1u)
#   warning "FSM_MAX_FLAGS is invalid, use default value 1."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static fsm_flag_t   *fsmFlagFreeList;               //! Head of event ocb pool
static fsm_flag_t    fsmFlagPool[FSM_MAX_FLAGS];    //! event ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_flag_init(void)
{
    uint_fast16_t n;
    fsm_flag_t **p;

    MEM_SET_ZERO((void *)fsmFlagPool, sizeof(fsmFlagPool));
    p = &fsmFlagFreeList;
    
    //! add event OCBs to the free list
    for (n = 0; n < ARRAY_LENGTH(fsmFlagPool); n++) {
        *p = &fsmFlagPool[n];
        p = (fsm_flag_t **)&((*p)->ObjNext);
    }
}

/*! \brief initialize task event
 *! \param pFlag event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
fsm_err_t fsm_flag_create  (fsm_handle_t   *pptEvent,
                            bool            bManualReset,
                            bool            bInitialState)
{
    uint8_t     flag;
    fsm_flag_t *pFlag;
    
    if (NULL == pptEvent) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (fsmIntNesting != 0) {
        return FSM_ERR_CREATE_ISR;
    }

    if (NULL == fsmFlagFreeList) {
        *pptEvent = NULL;
        return FSM_ERR_OBJ_DEPLETED;
    }
    pFlag           = fsmFlagFreeList;
    fsmFlagFreeList = (fsm_flag_t *)pFlag->ObjNext;
    
    flag = 0;
    if (bManualReset) {
        flag |= FSM_EVENT_MANUAL_RESET_BIT;
    }
    if (bInitialState) {
        flag |= FSM_EVENT_SINGNAL_BIT;
    }
    
    pFlag->ObjType      = FSM_OBJ_TYPE_FLAG;
    pFlag->ObjFlag      = 0u;
    pFlag->Head         = NULL;           
    pFlag->Tail         = NULL;
    pFlag->EventFlag    = flag;

    *pptEvent = pFlag;

    return FSM_ERR_NONE;
}

/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  Task parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
fsm_err_t fsm_flag_wait(fsm_handle_t hObject, uint32_t wTimeout)
{
    uint8_t         chResult;
    uint8_t         ObjType;
    fsm_tcb_t      *pTask = fsmScheduler.CurrentTask;
    fsm_flag_t     *pFlag = (fsm_flag_t *)hObject;

    if (NULL == hObject) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (fsmIntNesting != 0) {
        return FSM_ERR_PEND_ISR;
    }
    
    switch (pTask->Status) {
        case FSM_TASK_STATUS_READY:
            ObjType = ((fsm_basis_obj_t *)hObject)->ObjType;
            if (!(ObjType & FSM_OBJ_TYPE_WAITABLE)) {
                return FSM_ERR_OBJ_NOT_WAITABLE;
            }
            if (ObjType != FSM_OBJ_TYPE_FLAG) {
                return FSM_ERR_OBJ_TYPE;
            }
            
            SAFE_ATOM_CODE(
                if (pFlag->EventFlag & FSM_EVENT_SINGNAL_BIT) {
                    if (!(pFlag->EventFlag & FSM_EVENT_MANUAL_RESET_BIT)) {
                        pFlag->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
                    }
                    chResult = FSM_ERR_NONE;
                } else {
                    if (wTimeout == 0u) {
                        chResult = FSM_ERR_TASK_PEND_TIMEOUT;
                    } else {
                        //! add task to the object's wait queue.
                        pTask->Object = hObject;
                        fsm_set_task_pend(wTimeout);
                        fsm_waitable_obj_add_task(hObject, pTask);
                        chResult = FSM_ERR_OBJ_NOT_SINGLED;
                    }
                }
            )
            break;
            
        case FSM_TASK_STATUS_PEND_OK:
            pTask->Status    = FSM_TASK_STATUS_READY;
            chResult         = FSM_ERR_NONE;
            break;
        
        case FSM_TASK_STATUS_PEND_TIMEOUT:
            pTask->Status    = FSM_TASK_STATUS_READY;
            chResult         = FSM_ERR_TASK_PEND_TIMEOUT;
            break;
        
        case FSM_TASK_STATUS_PEND:
        default:
            chResult = FSM_ERR_OBJ_NOT_SINGLED;
            break;
    }

    return chResult;
}

/*! \brief set task event
 *! \param pFlag pointer for task event
 *! \return none
 */
fsm_err_t fsm_flag_set  (fsm_handle_t hObject) 
{
    fsm_flag_t  *pFlag = (fsm_flag_t *)hObject;
    fsm_tcb_t   *pTask;
    
    if (NULL == pFlag) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (pFlag->ObjType != FSM_OBJ_TYPE_FLAG) {
        return FSM_ERR_OBJ_TYPE;
    }
    
    SAFE_ATOM_CODE(
        pFlag->EventFlag |= FSM_EVENT_SINGNAL_BIT;
        if (pFlag->TaskQueue.Head != NULL) {
            if (!(pFlag->EventFlag & FSM_EVENT_MANUAL_RESET_BIT)) {
                pFlag->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
            }
            //! wake up all blocked tasks.
            while (pFlag->TaskQueue.Head != NULL) {
                pTask = fsm_waitable_obj_get_task(hObject);
                fsm_set_task_ready(pTask, FSM_TASK_STATUS_PEND_OK);
            }
        }
    )
        
    return FSM_ERR_NONE;
}

/*! \brief reset specified task event
 *! \param pFlag task event pointer
 *! \return none
 */
fsm_err_t fsm_flag_reset(fsm_handle_t hObject)
{
    fsm_flag_t *pFlag = (fsm_flag_t *)hObject;

    if (NULL == pFlag) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (pFlag->ObjType != FSM_OBJ_TYPE_FLAG) {
        return FSM_ERR_OBJ_TYPE;
    }
    
    SAFE_ATOM_CODE(
        pFlag->EventFlag &= ~FSM_EVENT_SINGNAL_BIT;
    )
        
    return FSM_ERR_NONE;
}
#endif

/* EOF */
