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
static fsm_semaphore_t   *sptSemtList;              //! Head of semaphore ocb pool
static fsm_semaphore_t    stSemPool[FSM_MAX_SEMS];       //! semaphore ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_semaphore_init(void)
{
    uint_fast8_t n;
    fsm_semaphore_t **p;

    MEM_SET_ZERO((void *)stSemPool, sizeof(stSemPool));
    p = &sptSemtList;
    
    //! add semaphore OCBs to the free list
    for (n = 0; n < UBOUND(stSemPool); n++) {
        *p = &stSemPool[n];
        p = (fsm_semaphore_t **)&((*p)->ObjNext);
    }
}

uint_fast8_t fsm_semaphore_create(
          fsm_semaphore_t **pptSem,
          uint16_t hwInitialCount,
          uint16_t hwMaximumCount)
{
    fsm_semaphore_t *ptSem;
    
    if ((NULL == pptSem)
    ||  (0 == hwMaximumCount)
    ||  (hwMaximumCount < hwInitialCount)) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    //!< get OCB from pool.
    if (NULL == sptSemtList) {
        *pptSem = NULL;
        return FSM_ERR_OBJ_NO_MORE_OCB;
    }
    
    ptSem        = sptSemtList;
    sptSemtList = (fsm_semaphore_t *)ptSem->ObjNext;
    
    SAFE_ATOM_CODE(
        ptSem->ObjType      = FSM_OBJ_TYPE_SEM;
        ptSem->ObjNext      = NULL;
        ptSem->Head      = NULL;           
        ptSem->Tail      = NULL;
        ptSem->SemCounter   = hwInitialCount; //!< set initial state
        ptSem->SemMaximum   = hwMaximumCount; //!< set initial state
        fsm_register_object(ptSem);             //!< register object.
    )
    *pptSem = ptSem;

    return FSM_ERR_NONE;
}

uint_fast8_t fsm_semaphore_release(fsm_semaphore_t *ptSem, uint16_t hwReleaseCount)
{
    uint8_t chError = FSM_ERR_NONE;
    
    if ((NULL == ptSem) || (0 == hwReleaseCount)) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (ptSem->ObjType != FSM_OBJ_TYPE_SEM) {
        return FSM_ERR_OBJ_TYPE_MISMATCHED;
    }
    
    SAFE_ATOM_CODE(
    do {
        fsm_tcb_t *pTask, *pNextTask;
        
        if ((ptSem->SemMaximum - ptSem->SemCounter) < hwReleaseCount) {
            chError = FSM_ERR_SEM_EXCEED;
            break;
        }
        ptSem->SemCounter += hwReleaseCount;
        
        if (ptSem->Head != NULL) {
            //! wake up blocked tasks.
            for (pTask = ptSem->Head;
                (NULL != pTask) && (0 != ptSem->SemCounter);
                pTask = pNextTask, ptSem->SemCounter--) {
                pNextTask = pTask->Next;
                fsm_set_task_ready(pTask);    //!< move task to ready list.
                pTask->Object = NULL;
                pTask->Status = FSM_TASK_STATUS_PEND_OK;
            }
            ptSem->Head = pTask;
            if (ptSem->Head == NULL) {
                ptSem->Tail = NULL;
            }
        }
    } while (0);
    )
        
    return chError;
}

#endif
/* EOF */
