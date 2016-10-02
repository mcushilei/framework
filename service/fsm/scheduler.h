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

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm_types.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name task queue control block
DEF_STRUCTURE(task_queue_t)
    task_ctrl_block_t   *pHead;         //!< queue head
    task_ctrl_block_t   *pTail;         //!< queue tail
#if TASK_QUEUE_POOL_SIZE > 1
    task_queue_t        *ptNext;
    uint8_t             chID;
#endif
END_DEF_STRUCTURE(task_queue_t)

//! \name scheduler
DEF_STRUCTURE(scheduler_t)
    task_ctrl_block_t   *ptCurrentTask;
#if TASK_QUEUE_POOL_SIZE > 1
    //! \brief free task queue list
    task_queue_t        *ptQHead;
    task_queue_t        *ptQTail;
    uint16_t            hwQueueBusyFlag;
#endif
END_DEF_STRUCTURE(scheduler_t)

/*============================ GLOBAL VARIABLES ==============================*/
extern scheduler_t g_tScheduler;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern void scheduler_init(void);
extern void scheduler_deinit(void);
extern bool scheduler_register_task(task_ctrl_block_t *pTask);
extern bool scheduler(void);

#if SAFE_TASK_THREAD_SYNC == ENABLED
extern bool scheduler_wait_for_single_object(event_t *pObject);
#endif      //! #if SAFE_TASK_THREAD_SYNC == ENABLED

#endif      //!< #ifndef __SCHEDULER_H__
/* EOF */
