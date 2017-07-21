/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
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

#ifndef __DRIVER_FREESCALE_MK20D5_H__
#define __DRIVER_FREESCALE_MK20D5_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\device.h"                           //!< MK20D5 CPU specification
#include "..\..\common\wdt\wdt.h"
#include "..\..\common\io\io.h"                 //!< io
#include "..\..\common\usart\usart.h"

//#include "..\common\adc\adc.h"                  //!< A/D Convertor
//#include "..\common\mcg\mcg.h"                  //!< power management
//#include "..\common\sleep\sleep.h"              //!< sleep 
//#include "..\common\flash\flash.h"              //!< flash
//#include "..\common\spi\spi.h"                  //!< spi


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief peripherals initialization
 *! \param none
 *! \return none
 */
extern bool driver_init(void);

#endif
/* EOF */