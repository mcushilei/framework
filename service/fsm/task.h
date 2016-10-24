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

#ifndef __TASK_H__
#define __TASK_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\fsm_types.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern bool task_stack_push(
                            fsm_tcb_t   *pTask,
                            state_func_t        *fnState,
                            void                *pArg);
extern bool task_stack_pop(fsm_tcb_t *pTask);

extern fsm_tcb_t *task_ctrl_block_new(
                            state_func_t        *fnState,
                            void                *pArg,
                            task_stack_item_t   *pStack,
                            uint_fast8_t        chStackSize);
extern void task_ctrl_block_free(fsm_tcb_t *pTask);
extern void task_ctrl_block_pool_init(void);
extern void task_ctrl_block_pool_deinit(void);

#endif      //!< #ifndef __TASK_H__
/* EOF */