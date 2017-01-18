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

#ifndef __SERVICE_H__
#define __SERVICE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\string\string.h"
#include ".\debug\debug.h"
#include ".\checksum\checksum.h"
#include ".\queue\queue.h"
#include ".\fifo\fifo.h"
#include ".\pool\pool.h"
#include ".\list\list.h"
#include ".\softtimer\softtimer.h"
#include ".\event_fsm\event_fsm.h"
#include ".\fsm\fsm.h"
#include ".\key\key.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*! \brief initialize all services
 *! \param none
 *! \retval TRUE initialize services succeed.
 *! \retval FALSE initialize services failed.
 */
extern bool service_init(void);

#endif
