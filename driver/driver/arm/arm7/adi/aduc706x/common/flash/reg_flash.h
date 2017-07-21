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

#ifndef __REG_FLASH_H__
#define __REG_FLASH_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define FLASH_REG                         (*(flash_reg_t *)FLASH_BASE_ADDRESS)

#define FLASH_REG_FEESTA (*(volatile uint32_t *)0xFFFF0E00)
#define FLASH_REG_FEEMOD (*(volatile uint32_t *)0xFFFF0E04)
#define FLASH_REG_FEECON (*(volatile uint32_t *)0xFFFF0E08)
#define FLASH_REG_FEEDAT (*(volatile uint32_t *)0xFFFF0E0C)
#define FLASH_REG_FEEADR (*(volatile uint32_t *)0xFFFF0E10)
#define FLASH_REG_FEESIG (*(volatile uint32_t *)0xFFFF0E18)
#define FLASH_REG_FEEPRO (*(volatile uint32_t *)0xFFFF0E1C)
#define FLASH_REG_FEEHID (*(volatile uint32_t *)0xFFFF0E20)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! ADC Register File
DEF_REGPAGE
    reg16_t FEESTA;
    REG16_RSVD(1)
        
    reg16_t FEEMOD;
    REG16_RSVD(1)
        
    reg8_t FEECON;
    REG8_RSVD(3)
        
    reg16_t FEEDAT;
    REG16_RSVD(1)
        
    reg16_t FEEADR;
    REG16_RSVD(1)

    REG32_RSVD(1)
        
    reg32_t FEESIG;
    reg32_t FEEPRO;
    reg32_t FEEHID;

END_DEF_REGPAGE(flash_reg_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
