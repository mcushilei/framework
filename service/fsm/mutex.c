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
static fsm_mutex_t   *sptMutexList;                     //! Head of mutex ocb pool
static fsm_mutex_t    stMutexPool[FSM_MAX_MUTEXES];     //! Mutex ocb pool

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void fsm_mutex_init(void)
{
    uint_fast8_t n;
    fsm_mutex_t **p;

    MEM_SET_ZERO((void *)stMutexPool, sizeof(stMutexPool));
    p = &sptMutexList;
    
    //! add mutex OCBs to the free list
    for (n = 0; n < UBOUND(stMutexPool); n++) {
        *p = &stMutexPool[n];
        p = (fsm_mutex_t **)&((*p)->ObjNext);
    }
}

uint_fast8_t fsm_mutex_create(
          fsm_mutex_t **pptMutex,
          bool bInitialOwner)
{
    uint8_t Flag;
    fsm_mutex_t *ptMutex;
    
    if (NULL == pptMutex) {
        return FSM_ERR_INVALID_PARAM;
    }

    //!< get OCB from pool.
    if (NULL == sptMutexList) {
        *pptMutex = NULL;
        return FSM_ERR_OBJ_NO_MORE_OCB;
    }
    
    ptMutex      = sptMutexList;
    sptMutexList = (fsm_mutex_t *)ptMutex->ObjNext;
    
    Flag = 0;
    if (bInitialOwner) {
        Flag |= FSM_MUTEX_OWNED_BIT;
    }
    
    SAFE_ATOM_CODE(
        ptMutex->ObjType      = FSM_OBJ_TYPE_MUTEX;
        ptMutex->ObjNext      = NULL;
        ptMutex->Head      = NULL;           
        ptMutex->Tail      = NULL;
        ptMutex->MutexFlag    = Flag;  //!< set initial state
        fsm_register_object(ptMutex);       //!< register object.
    )
    *pptMutex = ptMutex;

    return FSM_ERR_NONE;
}

uint_fast8_t fsm_mutex_release(fsm_mutex_t *ptMutex)
{
    if (NULL == ptMutex) {
        return FSM_ERR_INVALID_PARAM;
    }
    
    if (ptMutex->ObjType != FSM_OBJ_TYPE_MUTEX) {
        return FSM_ERR_OBJ_TYPE_MISMATCHED;
    }
    
    SAFE_ATOM_CODE(
        fsm_tcb_t *pTask;
        ptMutex->MutexFlag &= ~FSM_MUTEX_OWNED_BIT;
        //! wake up the first blocked task.
        pTask = fsm_task_dequeue(&ptMutex->TaskQueue);
        if (pTask != NULL) {
            ptMutex->MutexFlag |= FSM_MUTEX_OWNED_BIT;
            fsm_set_task_ready(pTask);    //!< move task to ready list.
            pTask->Object = NULL;
            pTask->Status = FSM_TASK_STATUS_PEND_OK;
        }
    )
        
    return FSM_ERR_NONE;
}

#endif
/* EOF */
