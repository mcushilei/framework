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


#ifndef __SPI_SD_FLASH_PORT_C__
#ifndef __SPI_SD_FLASH_PORT_H__
#define __SPI_SD_FLASH_PORT_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define SD_SPI_WRITE_BYTE(__W)      sd_spi_write_byte(__W)
#define SD_SPI_READ_BYTE()          sd_spi_read_byte()
#define SD_SPI_CS_SET()             (GPIO0_REG.FIOSET  = PIN16_MSK)
#define SD_SPI_CS_CLR()             (GPIO0_REG.FIOCLR  = PIN16_MSK)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern void sd_spi_write_byte(uint8_t chByte);
extern uint8_t sd_spi_read_byte(void);
extern uint8_t sd_crc7_calculator(uint8_t chCRCValue, uint8_t chData);
extern uint8_t sd_crc16_calculator(uint16_t hwCRCValue, uint8_t chData);

#endif  //! #ifndef __TEMPLATE_H__
#endif  //! #ifndef __TEMPLATE_C__
/* EOF */
