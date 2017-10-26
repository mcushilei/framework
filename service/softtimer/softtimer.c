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
typedef void timer_routine_t(void);

typedef struct {
    uint8_t         Flag;
    uint8_t         Ctrl;
    uint32_t        Count;
    uint32_t        Reload;
    timer_routine_t *pRoutine;
} softtimer_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static softtimer_t  softTimers[SOFTTIMER_MAX_TIMERS];

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool softtimer_init(void)
{
    return true;
}

bool softtimer_config(
                    uint8_t     timer,
                    uint32_t    initValue,
                    uint32_t    reloadValue,
                    timer_routine_t *pRoutine)
{
    if (timer >= SOFTTIMER_MAX_TIMERS) {
        return false;
    }
    __SOFTTIMER_SAFE_ATOME_CODE(
        softTimers[timer].Count  = initValue;
        softTimers[timer].Reload = reloadValue;
        softTimers[timer].Flag   = 0;
        softTimers[timer].pRoutine = pRoutine;
    )

    return true;
}

//! This function should be called periodly.
void softtimer_tick(void)
{
    uint_fast8_t n;

    for (n = 0; n < ARRAY_LENGTH(softTimers); n++) {
        __SOFTTIMER_SAFE_ATOME_CODE(
            if (softTimers[n].Count != 0u) {
                softTimers[n].Count--;
                if (softTimers[n].Count == 0) {
                    softTimers[n].Flag |= BIT(0);
                    softTimers[n].Count = softTimers[n].Reload;
                    if (softTimers[n].pRoutine != NULL) {
                        softTimers[n].pRoutine();
                    }
                }
            }
        )
    }
}

void softtimer_start(uint8_t Timer, uint32_t Value)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    __SOFTTIMER_SAFE_ATOME_CODE(
        softTimers[Timer].Count = Value;
        softTimers[Timer].Flag &= ~BIT(0);
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

bool softtimer_is_timeout(uint8_t Timer)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
        if (softTimers[Timer].Flag) {
            __SOFTTIMER_SAFE_ATOME_CODE(
                softTimers[Timer].Flag &= ~BIT(0);
            )
            return true;
        }
    }
    
    return false;
}


/* EOF */
