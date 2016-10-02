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


#ifndef __DRIVER_ARM_M0_NXP_LPC1227_H__
#define __DRIVER_ARM_M0_NXP_LPC1227_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\device.h"
#include "..\common\scon\pm.h"
#include "..\common\uart\uart.h"
#include "..\common\spi\spi.h"
#include "..\common\i2c\i2c.h"
#include "..\common\io\io.h"
#include "..\common\wdt\wdt.h"
#include "..\common\flash\flash.h"
#include "..\common\rtc\rtc.h"
//#include "..\common\timer\timer.h"
#include "..\common\adc\adc.h"

/*============================ MACROS ========================================*/

#define PSC_UID0    PSC_UID[0]
#define PSC_UID1    PSC_UID[1]
#define PSC_UID2    PSC_UID[2]
#define PSC_UID3    PSC_UID[3]

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern ROOT uint32_t PSC_UID[4];

/*! \note initialize driver
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */
extern bool driver_init( void );

#endif
/* EOF */