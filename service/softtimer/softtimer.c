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
#define __SOFTTIMER_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void fn_softtimer_handler_t(uint8_t Timer);

DEF_STRUCTURE(softtimer_t)
    uint32_t        Count;
    uint32_t        Reload;
    uint8_t         Flag;
    fn_softtimer_handler_t *pHandler;
END_DEF_STRUCTURE(softtimer_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static softtimer_t  softTimers[SOFTTIMER_MAX_TIMERS];

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void softtimer_init(uint8_t     Timer,
                    uint32_t    Value,
                    uint32_t    Reload,
                    fn_softtimer_handler_t *pHandler)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    __SOFTTIMER_SAFE_ATOME_CODE(
        softTimers[Timer].Count  = Value;
        softTimers[Timer].Reload = Reload;
        softTimers[Timer].Flag   = 0;
        softTimers[Timer].pHandler = pHandler;
    )
    }
}

//! This function should be called periodly.
void softtimer_tick(void)
{
    uint_fast8_t n;

    for (n = 0; n < ARRAY_LENGTH(softTimers); n++) {
        if (softTimers[n].Count) {
            softTimers[n].Count--;
            if (softTimers[n].Count == 0) {
                softTimers[n].Flag = 1;
                softTimers[n].Count = softTimers[n].Reload;
                if (softTimers[n].pHandler != NULL) {
                    softTimers[n].pHandler(n);
                }
            }
        }
    }
}

void softtimer_start(uint8_t Timer, uint32_t Value)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    __SOFTTIMER_SAFE_ATOME_CODE(
        softTimers[Timer].Count = Value;
        softTimers[Timer].Flag = 0;
    )
    }
}

void softtimer_stop(uint8_t Timer)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    __SOFTTIMER_SAFE_ATOME_CODE(
        softTimers[Timer].Count = 0;
    )
    }
}

bool softtimer_check_timeout(uint8_t Timer)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
        if (softTimers[Timer].Flag) {
            __SOFTTIMER_SAFE_ATOME_CODE(
                softTimers[Timer].Flag = 0;
            )
            return true;
        }
    }
    
    return false;
}


/* EOF */
