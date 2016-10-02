/***************************************************************************
 *   Copyright(C)2009-2014 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
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

//! \note do not move this pre-processor statement to other places
#include "..\app_cfg.h"

#ifndef __FSM_APP_CFG_H__
#define __FSM_APP_CFG_H__

/*============================ INCLUDES ======================================*/
#include "..\string\string.h"

/*============================ MACROS ========================================*/
#define TASK_TCB_POOL_SIZE      (16u)
#define TASK_QUEUE_POOL_SIZE    (4u)
#define SAFE_TASK_THREAD_SYNC   ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
#define MEM_SET_ZERO(__P, __S)  mem_set(__P, 0, __S)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif  //!< #define __FSM_APP_CFG_H__
/* EOF */
