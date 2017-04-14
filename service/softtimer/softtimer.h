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
typedef void timer_routine_t(void *pArg);

DEF_STRUCTURE(softtimer_t)
    uint32_t        Count;
    uint32_t        Reload;
    uint8_t         Flag;
    timer_routine_t *pRoutine;
    void            *pRoutineArg;
    END_DEF_STRUCTURE(softtimer_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
bool softtimer_init(void);
void softtimer_config(
                    uint8_t     Timer,
                    uint32_t    Value,
                    uint32_t    Reload,
                    timer_routine_t *pRoutine,
                    void            *pArg);
void softtimer_tick(void);
void softtimer_start(uint8_t Timer, uint32_t Value);
void softtimer_stop(uint8_t Timer);
bool softtimer_check_timeout(uint8_t Timer);

#endif  //! #ifndef __SOFTTIMER_H__
#endif  //! #ifndef __SOFTTIMER_C__
/* EOF */
