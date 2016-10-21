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
#define __FSM_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\task.h"
#include ".\scheduler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief fsm init.
 */
bool fsm_init(void)
{
    task_ctrl_block_pool_init();
    scheduler_init();

    return true;
}

/*! \brief fsm init.
 */
bool fsm_deinit(void)
{
    scheduler_deinit();
    task_ctrl_block_pool_deinit();

    return true;
}

/*! \brief perform a state transfer.
 *  \param pT a pointer of task control block
 *  \param fnState target state
 *  \param pArg a pointer of argument control block
 *  \retval false invalid param
 *  \retval true succeeded to transfer state
 */
bool fsm_state_transfer(
                state_func_t *      fnState,
                void *              pArg)
{
    task_ctrl_block_t *pTask = g_tScheduler.ptCurrentTask;
    
    if (NULL == pTask) {
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        ptRoutine->fnState  = fnState;
        ptRoutine->pArg     = pArg;
    } while (false);

    return true;
}

/*! \brief call a sub task routine.
 *  \param pTask a pointer of task control block
 *  \param fnState target routine
 *  \param pArg a pointer of argument control block
 *  \param fnReturnRoutine return to this routine when sub FSM completed
 *  \param pReturnArg argument for return routine
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
bool fsm_call_sub_ex(
                state_func_t *      fnState,
                void *              pArg,
                state_func_t *      fnReturnRoutine,
                void *              pReturnArg)
{
    task_ctrl_block_t *pTask = g_tScheduler.ptCurrentTask;

    if ((NULL == pTask)
    ||  (NULL == fnState)
    ||  (NULL == fnReturnRoutine)) {
        return false;
    }

    if (!task_stack_push(pTask, fnReturnRoutine, pReturnArg)) {
        //!< fatal error! stack is overflow.
        while (task_stack_pop(pTask));  //!< Terminate this task!
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        ptRoutine->fnState  = fnState;
        ptRoutine->pArg     = pArg;
    } while (false);

    return true;
}

/*! \brief call a sub routine and return current state when sub FSM complete.
 *  \param pT a pointer of task control block
 *  \param fnState target routine
 *  \param pArg a pointer of argument control block
 *  \retval false invalid param or stack overflow
 *  \retval true succeeded to call sub FSM
 */
bool fsm_call_sub(
                state_func_t *      fnState, 
                void *              pArg)
{
    task_ctrl_block_t *pTask = g_tScheduler.ptCurrentTask;

    if (NULL == pTask) {
        return false;
    }

    do {
        task_stack_item_t *ptRoutine = pTask->pStack;

        return fsm_call_sub_ex(
                fnState, pArg, ptRoutine->fnState, ptRoutine->pArg);
    } while (false);
}

/*! \brief  create a new task control block
 *  \param  fnState task routine
 *  \param  fnReturnTo a routine which will be called when task routine return
 *          false.
 *  \param  pArg a pointer of a argument control block
 *  \param  pStack a return stack buffer
 *  \param  chStackSize return stack size
 *  \return task handle
 */
task_ctrl_block_t *fsm_create_task(
            state_func_t *      fnState,
            void *              pArg,
            task_stack_item_t * pStack,
            uint_fast8_t        chStackSize)
{
    /*! try to get a new task control block */
    task_ctrl_block_t *pTask = task_ctrl_block_new(fnState, pArg, pStack, chStackSize);
    if (NULL != pTask) {
        if (scheduler_register_task(pTask)) {
            return pTask;
        }
        task_ctrl_block_free(pTask);
    }

    return NULL;
}

/* EOF */
