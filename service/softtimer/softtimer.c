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
typedef void fn_softtimer_handler_t(uint8_t chTimer);

DEF_STRUCTURE(softtimer_t)
    uint32_t        wCount;
    uint32_t        wReload;
    uint8_t         chFlag;
    fn_softtimer_handler_t *fnHandler;
END_DEF_STRUCTURE(softtimer_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static softtimer_t  stSofttimers[SOFTTIMER_MAX_TIMERS];

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void softtimer_init(uint8_t     chTimer,
                    uint32_t    wValue,
                    uint32_t    wReload,
                    uint8_t     chFlag,
                    fn_softtimer_handler_t *fnHandler)
{
    if (chTimer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[chTimer].wCount  = wValue;
        stSofttimers[chTimer].wReload = wReload;
        stSofttimers[chTimer].chFlag  = 0;
        stSofttimers[chTimer].fnHandler = fnHandler;
    )
    }
}

void softtimer_tick(void)
{
    uint_fast8_t n;

    for (n = 0; n < UBOUND(stSofttimers); n++) {
        if (stSofttimers[n].wCount) {
            stSofttimers[n].wCount--;
            if (stSofttimers[n].wCount == 0) {
                stSofttimers[n].chFlag = 1;
                if (stSofttimers[n].fnHandler != NULL) {
                    stSofttimers[n].fnHandler(n);
                }
            }
        }
    }
}

void softtimer_start(uint8_t chTimer, uint32_t wValue)
{
    if (chTimer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[chTimer].wCount = wValue;
        stSofttimers[chTimer].chFlag = 0;
    )
    }
}

void softtimer_stop(uint8_t chTimer)
{
    if (chTimer < SOFTTIMER_MAX_TIMERS) {
    SAFE_ATOM_CODE(
        stSofttimers[chTimer].wCount = 0;
    )
    }
}

bool softtimer_is_timeout(uint8_t chTimer)
{
    if (chTimer < SOFTTIMER_MAX_TIMERS) {
        if (stSofttimers[chTimer].chFlag) {
            SAFE_ATOM_CODE(
                stSofttimers[chTimer].chFlag = 0;
            )
            return true;
        }
    }
    
    return false;
}


/* EOF */
