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
#if ((!defined(FSM_MAX_MUTEXES)) || (FSM_MAX_MUTEXES < 1))
#   define FSM_MAX_MUTEXES           (1u)
#   warning "FSM_MAX_MUTEXES is invalid, use default value 1."
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static fsm_mutex_t   *fsmMutexList;                     //! Head of mutex ocb pool
static fsm_mutex_t    fsmMutexPool[FSM_MAX_MUTEXES];    //! Mutex ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_mutex_init(void)
{
    uint_fast16_t n;
    fsm_mutex_t **p;

    p = &fsmMutexList;
    
    //! add mutex OCBs to the free list
    for (n = 0; n < ARRAY_LENGTH(fsmMutexPool); n++) {
        *p = &fsmMutexPool[n];
        p = (fsm_mutex_t **)&((*p)->ObjNext);
    }
}

fsm_err_t    fsm_mutex_create  (fsm_handle_t *pptMutex)
{
    fsm_mutex_t *ptMutex;
    
    if (NULL == pptMutex) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (fsmIntNesting != 0) {
        return FSM_ERR_CREATE_ISR;
    }

    if (NULL == fsmMutexList) {
        *pptMutex = NULL;
        return FSM_ERR_OBJ_DEPLETED;
    }
    ptMutex      = fsmMutexList;
    fsmMutexList = (fsm_mutex_t *)ptMutex->ObjNext;
    
    ptMutex->ObjType    = FSM_OBJ_TYPE_MUTEX;
    ptMutex->ObjFlag    = 0u;
    ptMutex->Head       = NULL;           
    ptMutex->Tail       = NULL;
    ptMutex->MutexOwner = NULL;

    *pptMutex = ptMutex;

    return FSM_ERR_NONE;
}

/*! \brief  wait for a specified task event
 *! \param  pObject target event item
 *! \param  Task parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
fsm_err_t fsm_mutex_wait(fsm_handle_t hObject, uint32_t wTimeout)
{
    uint8_t         chResult;
    uint8_t         ObjType;
    fsm_tcb_t      *pTask = fsmScheduler.CurrentTask;
    fsm_mutex_t    *ptMutex = (fsm_mutex_t *)hObject;

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
            if (ObjType != FSM_OBJ_TYPE_MUTEX) {
                return FSM_ERR_OBJ_TYPE;
            }
            
            chResult = FSM_ERR_OBJ_NOT_SINGLED;
            if (ptMutex->MutexOwner != NULL) {
                if (ptMutex->MutexOwner == pTask) {
                    chResult = FSM_ERR_NONE;
                } else if (wTimeout == 0u) {
                    chResult = FSM_ERR_TASK_PEND_TIMEOUT;
                } else {
                    //! add task to the object's wait queue.
                    pTask->Object = hObject;
                    fsm_set_task_pend(wTimeout);
                    fsm_waitable_obj_add_task(hObject, pTask);
                    chResult = FSM_ERR_OBJ_NOT_SINGLED;
                }
            } else {
                ptMutex->MutexOwner = pTask;
                chResult = FSM_ERR_NONE;
            }
            break;
            
        case FSM_TASK_STATUS_PEND_OK:
            pTask->Status    = FSM_TASK_STATUS_READY;
            chResult         = FSM_ERR_NONE;
            break;
            
        case FSM_TASK_STATUS_PEND_TIMEOUT:
            pTask->Status    = FSM_TASK_STATUS_READY;
            chResult        = FSM_ERR_TASK_PEND_TIMEOUT;
            break;
            
        case FSM_TASK_STATUS_PEND:
        default:
            chResult = FSM_ERR_OBJ_NOT_SINGLED;
            break;
    }

    return chResult;
}

fsm_err_t fsm_mutex_release(fsm_handle_t hObject)
{
    fsm_mutex_t *ptMutex = (fsm_mutex_t *)hObject;
    fsm_tcb_t   *pTask = fsmScheduler.CurrentTask;

    if (NULL == ptMutex) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (fsmIntNesting != 0) {
        return FSM_ERR_CALL_IN_ISR;
    }
    
    if (ptMutex->ObjType != FSM_OBJ_TYPE_MUTEX) {
        return FSM_ERR_OBJ_TYPE;
    }
    
    if (pTask != ptMutex->MutexOwner) {
        return FSM_ERR_NOT_MUTEX_OWNER;
    }
    
    //! wake up the first blocked task.
    pTask = fsm_waitable_obj_get_task(hObject);
    if (pTask != NULL) {
        ptMutex->MutexOwner = pTask;
        fsm_set_task_ready(pTask, FSM_TASK_STATUS_PEND_OK);
    } else {
        ptMutex->MutexOwner = NULL;
    }
        
    return FSM_ERR_NONE;
}

#endif
/* EOF */
