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


#ifndef __IAP_H__
#define __IAP_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern uint32_t iap_flash_write_pages(uint32_t wStartPage, uint32_t *pBuffer, uint32_t cnt);
extern uint32_t iap_flash_erase_pages(uint32_t wStartPage, uint32_t cnt);
extern uint32_t iap_eeprom_read(uint32_t Addr, uint8_t *pBuffer, uint32_t Length);
extern uint32_t iap_eeprom_write(uint32_t Addr, uint8_t *pBuffer, uint32_t Length);

#endif
/* EOF */
