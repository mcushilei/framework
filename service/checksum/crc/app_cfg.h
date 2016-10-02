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

#ifndef __SERVICE_CHECKSUM_CRC_APP_CFG_H__
#define __SERVICE_CHECKSUM_CRC_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define CRC7_POLY_TELECOM       (0x09)
#define CRC7_POLY               CRC7_POLY_TELECOM

#define CRC8_POLY_STAND         (0xD5)
#define CRC8_POLY_CCITT         (0x07)
#define CRC8_POLY_DALLAS_MAXIM  (0x31)
#define CRC8_POLY_SAE_J1850     (0x1D)
#define CRC8_POLY_WCDMA         (0x9B)
#define CRC8_POLY               CRC8_POLY_DALLAS_MAXIM

#define CRC16_POLY_ARINC        (0xA02B)
#define CRC16_POLY_CCITT        (0x1021)
#define CRC16_POLY_CDMA2000     (0xC867)
#define CRC16_POLY_DECT         (0x0589)
#define CRC16_POLY_T10_DIF      (0x8BB7)
#define CRC16_POLY_DNP          (0x3D65)
#define CRC16_POLY_IBM          (0x8005)
#define CRC16_POLY              CRC16_POLY_CCITT

#define CRC32_POLY_IEEE802_3    (0x04C11DB7u)
#define CRC32_POLY              CRC32_POLY_IEEE802_3

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
