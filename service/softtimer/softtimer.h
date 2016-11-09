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


#ifndef __SOFTTIMER_C__
#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void fn_softtimer_handler_t(uint8_t chTimer);

DEF_STRUCTURE(softtimer_t)
    uint32_t        wReload;
    uint32_t        wCount;
    uint8_t         chFlag;
    fn_softtimer_handler_t *fnHandler;
END_DEF_STRUCTURE(softtimer_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
void softtimer_init(uint8_t     chTimer,
                    uint32_t    wValue,
                    uint32_t    wReload,
                    uint8_t     chFlag,
                    fn_softtimer_handler_t *fnHandler);
void softtimer_tick(void);
void softtimer_start(uint8_t chTimer, uint32_t wValue);
void softtimer_stop(uint8_t chTimer);
bool softtimer_is_timeout(uint8_t chTimer);

#endif  //! #ifndef __SOFTTIMER_H__
#endif  //! #ifndef __SOFTTIMER_C__
/* EOF */
