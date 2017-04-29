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
#if ((!defined(FSM_MAX_SEMS)) || (FSM_MAX_SEMS < 1))
#   define FSM_MAX_SEMS           (1u)
#   warning "FSM_MAX_SEMS is invalid, use default value 1."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static fsm_semaphore_t   *fsmSemtList;              //! Head of semaphore ocb pool
static fsm_semaphore_t    fsmSemPool[FSM_MAX_SEMS]; //! semaphore ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_semaphore_init(void)
{
    uint_fast16_t n;
    fsm_semaphore_t **p;

    p = &fsmSemtList;
    
    //! add semaphore OCBs to the free list
    for (n = 0; n < ARRAY_LENGTH(fsmSemPool); n++) {
        *p = &fsmSemPool[n];
        p = (fsm_semaphore_t **)&((*p)->ObjNext);
    }
}

fsm_err_t fsm_semaphore_create(
          fsm_handle_t     *pptSem,
          uint16_t          hwInitialCount)
{
    fsm_semaphore_t *ptSem;
    
    if (NULL == pptSem) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (fsmIntNesting != 0) {
        return FSM_ERR_CREATE_ISR;
    }
    
    if (NULL == fsmSemtList) {
        *pptSem = NULL;
        return FSM_ERR_OBJ_DEPLETED;
    }
    ptSem       = fsmSemtList;
    fsmSemtList = (fsm_semaphore_t *)ptSem->ObjNext;
    
    ptSem->ObjType      = FSM_OBJ_TYPE_SEM;
    ptSem->ObjFlag      = 0u;
    ptSem->Head         = NULL;           
    ptSem->Tail         = NULL;
    ptSem->SemCounter   = hwInitialCount;
    
    *pptSem = ptSem;

    return FSM_ERR_NONE;
}

/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  Task parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
fsm_err_t fsm_semaphore_wait(fsm_handle_t hObject, uint32_t wTimeout)
{
    uint8_t             chResult;
    uint8_t             ObjType;
    fsm_tcb_t          *pTask = fsmScheduler.CurrentTask;
    fsm_semaphore_t    *ptSem = (fsm_semaphore_t *)hObject;

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
            if (ObjType != FSM_OBJ_TYPE_SEM) {
                return FSM_ERR_OBJ_TYPE;
            }
            
            FSM_SAFE_ATOM_CODE(
                if (ptSem->SemCounter == 0) {
                    if (wTimeout == 0u) {
                        chResult = FSM_ERR_TASK_PEND_TIMEOUT;
                    } else {
                        //! add task to the object's wait queue.
                        pTask->Object = hObject;
                        fsm_set_task_pend(wTimeout);
                        fsm_waitable_obj_add_task(hObject, pTask);
                        chResult = FSM_ERR_OBJ_NOT_SINGLED;
                    }
                } else {
                    ptSem->SemCounter--;
                    chResult = FSM_ERR_NONE;
                }
            )
            break;
            
        case FSM_TASK_STATUS_PEND_OK:
            pTask->Status   = FSM_TASK_STATUS_READY;
            chResult        = FSM_ERR_NONE;
            break;
        
        case FSM_TASK_STATUS_PEND_TIMEOUT:
            pTask->Status   = FSM_TASK_STATUS_READY;
            chResult        = FSM_ERR_TASK_PEND_TIMEOUT;
            break;
        
        case FSM_TASK_STATUS_PEND:
        default:
            chResult = FSM_ERR_OBJ_NOT_SINGLED;
            break;
    }

    return chResult;
}

fsm_err_t fsm_semaphore_release(fsm_handle_t hObject, uint16_t hwReleaseCount)
{
    fsm_semaphore_t *ptSem = (fsm_semaphore_t *)hObject;
    fsm_tcb_t       *pTask;
    
    if ((NULL == ptSem) || (0 == hwReleaseCount)) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (ptSem->ObjType != FSM_OBJ_TYPE_SEM) {
        return FSM_ERR_OBJ_TYPE;
    }
    
    FSM_SAFE_ATOM_CODE(
        if (hwReleaseCount <= (65535u - ptSem->SemCounter)) {
            ptSem->SemCounter += hwReleaseCount;
        }
        
        //! wake up blocked tasks.
        while ((NULL != ptSem->Head) && (0 != ptSem->SemCounter)) {
            ptSem->SemCounter--;
            pTask = fsm_waitable_obj_get_task(hObject);
            fsm_set_task_ready(pTask, FSM_TASK_STATUS_PEND_OK);
        }
    )
        
    return FSM_ERR_NONE;
}

#endif
/* EOF */
