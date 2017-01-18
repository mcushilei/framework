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
#include "..\app_cfg.h"

#ifndef __COMMUNICATION_FRAME_APP_CFG_H__
#define __COMMUNICATION_FRAME_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define FRAME_PAYLOAD_MAX_SIZE          (8u)

#define FRAME_HEAD_SEGMENT_SIZE         (2u)
#define FRAME_LENGTH_SEGMENT_SIZE       (1u)
#define FRAME_CHECKSUM_SEGMENT_SIZE     (1u)

#define FRAM_CRC8_POLLY         CRC8_POLY_CCITT
#define FRAM_CRC16_POLLY        CRC16_POLY_CCITT

#define FRAME_RCV_HEAD_BYTE_0     (0xA5)
#define FRAME_RCV_HEAD_BYTE_1     (0x5A)

#define FRAME_SND_HEAD_BYTE_0     (0xA5)
#define FRAME_SND_HEAD_BYTE_1     (0x5A)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __COMMUNICATION_FRAME_APP_CFG_H__
/* EOF */
