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

//! \note do not move this pre-processor statement to other places
#define __SPI_SD_FLASH_PORT_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void sd_spi_write_byte(uint8_t chByte)
{
    uint16_t hwTmp = chByte;
    
    spi_write(hwTmp);
    while (!spi_read(&hwTmp));
}

uint8_t sd_spi_read_byte(void)
{
    uint16_t hwTmp = 0xFFu;
    
    spi_write(hwTmp);
    while (!spi_read(&hwTmp));
    
    return (uint8_t)hwTmp;
}

uint8_t sd_crc7_calculator(uint8_t chCRCValue, uint8_t chData)
{
    return crc7_calculator(CRC7_POLY_TELECOM, chCRCValue, chData);
}

uint8_t sd_crc16_calculator(uint16_t hwCRCValue, uint8_t chData)
{
    return crc16_calculator(CRC16_POLY_CCITT, hwCRCValue, chData);
}

bool spi_sd_enable(void)
{
    return true;
}

bool spi_sd_disable(void)
{
    return true;
}

/* EOF */
