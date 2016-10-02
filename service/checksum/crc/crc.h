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


#ifndef __CRC_H__
#define __CRC_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define CRC32_IEEE802_3_CHECKSUM_INIT   (0xFFFFFFFFul)

/*============================ MACROFIED FUNCTIONS ===========================*/
#define CRC7(__CRCVAL, __NEWCHAR)       crc7_calculator(CRC7_POLY, (__CRCVAL), (__NEWCHAR))
#define CRC8(__CRCVAL, __NEWCHAR)       crc8_calculator(CRC8_POLY, (__CRCVAL), (__NEWCHAR))
#define CRC16(__CRCVAL, __NEWCHAR)      crc16_calculator(CRC16_POLY, (__CRCVAL), (__NEWCHAR))
#define CRC32(__CRCVAL, __NEWCHAR)      crc32_calculator(CRC32_POLY, (__CRCVAL), (__NEWCHAR))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern uint8_t  crc7_calculator(uint8_t chPoly, uint8_t chCRCValue, uint8_t chData);
extern void     crc7_table_generator(uint8_t chPoly, uint8_t *pchTable);
extern uint8_t  crc7_check(uint8_t chCRCValue, uint8_t chData, const uint8_t *pchTable);
extern uint8_t  crc8_calculator(uint8_t chPoly, uint8_t chCRCValue, uint8_t chData);
extern void     crc8_table_generator(uint8_t chPoly, uint8_t *pchTable);
extern uint8_t  crc8_check(uint8_t chCRCValue, uint8_t chData, const uint8_t *pchTable);
extern uint16_t crc16_calculator(uint16_t hwPoly, uint16_t hwCRCValue, uint8_t chData);
extern void     crc16_table_generator(uint16_t hwPoly, uint16_t *phwTable);
extern uint16_t crc16_check(uint16_t hwCRCValue, uint8_t chData, const uint16_t *phwTable);
extern uint32_t crc32_calculator(uint32_t wPoly, uint32_t wCRCValue, uint8_t chData);
extern void     crc32_table_generator(uint32_t wPoly, uint32_t *pwTable);
extern uint32_t crc32_check(uint32_t wCRCValue, uint8_t chData, const uint32_t *pwTable);

#endif
