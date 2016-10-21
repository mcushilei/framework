/***************************************************************************
 *   Copyright(C)2009-2010 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm_types.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static task_ctrl_block_t *spTCBPool = NULL;             //! Head of task control block pool
static task_ctrl_block_t stTCBPool[TASK_TCB_POOL_SIZE]; //! task control block pool

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief try to push a routine into stack
 *  \param pTask
 *  \param fnState routine
 *  \param pArg argument for routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
bool task_stack_push(
        task_ctrl_block_t   *pTask,
        state_func_t        *fnState,
        void                *pArg) 
{
    //! validate parameters.
    if ((NULL == pTask)
    ||  (NULL == pTask->pStack)
    ||  (NULL == fnState)) {
        return false;
    }

    //! if stack full
    if (pTask->chStackLevel >= pTask->chStackSize) {
        return false;
    }

    do {
        task_stack_item_t *ptStack = pTask->pStack;

        ptStack->fnState    = fnState;
        ptStack->pArg       = pArg;
    } while (false);
    pTask->pStack++;
    pTask->chStackLevel++;

    return true;
}

/*! \brief  try to pop a routine from stack
 *  \param  pTask
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
bool task_stack_pop(task_ctrl_block_t *pTask)
{
    //! validate parameters.
    if ((NULL == pTask)
    ||  (NULL == pTask->pStack)) {
        return false;
    }

    //! if stack empty
    if (0 == pTask->chStackLevel) {
        return false;
    }

    pTask->pStack--;
    pTask->chStackLevel--;

    return true;
}

/*! \brief get a new task control block from pool and initial it
 *  \param fnState task's first routine
 *  \param pArg a pointer of a argument
 *  \param pStack a return stack buffer
 *  \param chStackSize return stack size
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
task_ctrl_block_t *task_ctrl_block_new(
        state_func_t        *fnState,
        void                *pArg,
        task_stack_item_t   *pStack,
        uint_fast8_t        chStackSize)
{
    task_ctrl_block_t *pTCB = NULL;

    if ((NULL == fnState)                       //! validate parameters.
    ||  (NULL == pStack) 
    ||  (0 == chStackSize)) {
        return NULL;
    }
    
    if (NULL != spTCBPool) {
        pTCB = spTCBPool;
        spTCBPool = spTCBPool->pNext;
    }

    if (NULL != pTCB) {                         //!< find a task control block ?
        pTCB->pNext            = NULL;

        pTCB->pStack           = pStack;        //!< set stack buffer
        pTCB->pStack->fnState  = fnState;       //!< set task routine    
        pTCB->pStack->pArg     = pArg;          //!< set argument
        pTCB->chStackSize      = chStackSize;   //!< set stack size
        pTCB->chStackLevel     = 0;             //!< set stack point

#if SAFE_TASK_THREAD_SYNC == ENABLED
        pTCB->pObject          = NULL;
#endif
    }

    return pTCB;
}

/*! \brief  free a task control blcok to pool
 *  \param  pTCB a pointer of a task control block
 *  \return none
 */
void task_ctrl_block_free(task_ctrl_block_t *pTCB)
{
    if (NULL == pTCB) {
        return;
    }

    MEM_SET_ZERO((void *)pTCB, sizeof(task_ctrl_block_t));

    pTCB->pNext  = spTCBPool;        //! add task item to freelist
    spTCBPool   = pTCB;
}

/*! \brief  add task control blocks to the task pool
 *  \param  pHead task heap buffer
 *  \param  hwSize heap size
 *  \return access result
 */
void task_ctrl_block_pool_init(void)
{
    uint_fast8_t n = 0;
    task_ctrl_block_t *p = stTCBPool;

    MEM_SET_ZERO((void *)stTCBPool, sizeof(stTCBPool));

    //! add tasks to the free list
    for (n = UBOUND(stTCBPool); n; n--) {
        p->pNext  = spTCBPool;    //! add task item to freelist
        spTCBPool   = p;
        p++;
    }
}

/*! \brief  
 *  \param  none
 *  \return access result
 */
void task_ctrl_block_pool_deinit(void)
{
    MEM_SET_ZERO((void *)stTCBPool, sizeof(stTCBPool));

    spTCBPool = NULL;
}


/* EOF */
