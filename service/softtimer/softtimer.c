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
    fn_softtimer_handler_t *Handler;
END_DEF_STRUCTURE(softtimer_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static softtimer_t  stSofttimers[SOFTTIMER_MAX_TIMERS];

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void softtimer_init(uint8_t     Timer,
                    uint32_t    Value,
                    uint32_t    Reload,
                    uint8_t     Flag,
                    fn_softtimer_handler_t *Handler)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[Timer].Count  = Value;
        stSofttimers[Timer].Reload = Reload;
        stSofttimers[Timer].Flag  = 0;
        stSofttimers[Timer].Handler = Handler;
    )
    }
}

void softtimer_tick(void)
{
    uint_fast8_t n;

    for (n = 0; n < UBOUND(stSofttimers); n++) {
        if (stSofttimers[n].Count) {
            stSofttimers[n].Count--;
            if (stSofttimers[n].Count == 0) {
                stSofttimers[n].Flag = 1;
                if (stSofttimers[n].Handler != NULL) {
                    stSofttimers[n].Handler(n);
                }
            }
        }
    }
}

void softtimer_start(uint8_t Timer, uint32_t Value)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[Timer].Count = Value;
        stSofttimers[Timer].Flag = 0;
    )
    }
}

void softtimer_stop(uint8_t Timer)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[Timer].Count = 0;
    )
    }
}

bool softtimer_is_timeout(uint8_t Timer)
{
    if (Timer < SOFTTIMER_MAX_TIMERS) {
        if (stSofttimers[Timer].Flag) {
            SAFE_ATOM_CODE(
                stSofttimers[Timer].Flag = 0;
            )
            return true;
        }
    }
    
    return false;
}


/* EOF */
