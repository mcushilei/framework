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
#define __PIPE_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define this            (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(c_pipe_t, 
        typedef bool fn_output_byte_t(uint8_t chByte);
    )
    const uint8_t *pchData;
    fn_output_byte_t *fnOutByte;
    uint32_t      wAmount;
    uint32_t      wCount;
END_DEF_CLASS(c_pipe_t)

DEF_INTERFACE(i_pipe_t)
    bool     (*Init)(const uint8_t *pchData, uint32_t wAmount, fn_output_byte_t *fnOutByte, c_pipe_t *ptPipe);
    fsm_rt_t (*Out)(c_pipe_t *ptPipe);
END_DEF_INTERFACE(i_pipe_t)

/*============================ PROTOTYPES ====================================*/
static bool pipe_init(const uint8_t *pchData, uint32_t wAmount, fn_output_byte_t *fnOutByte, c_pipe_t *ptPipe);
static fsm_rt_t pipe_out(c_pipe_t *ptPipe);

/*============================ GLOBAL VARIABLES ==============================*/
const i_pipe_t PIPE = {
    .Init = &pipe_init,
    .Out  = &pipe_out,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
bool pipe_init(const uint8_t *pchData, uint32_t wAmount, fn_output_byte_t *fnOutByte, c_pipe_t *ptPipe)
{
    CLASS(c_pipe_t) *ptThis = (CLASS(c_pipe_t) *)ptPipe;

    if ((NULL == pchData) || (NULL == fnOutByte) || (NULL == ptPipe)) {
        return false;
    }

    this.pchData   = pchData;
    this.wAmount   = wAmount;
    this.wCount    = 0;
    this.fnOutByte = fnOutByte;

    return true;
}


fsm_rt_t pipe_out(c_pipe_t *ptPipe)
{
    CLASS(c_pipe_t) *ptThis = (CLASS(c_pipe_t) *)ptPipe;

    if (NULL == ptPipe) {
        return fsm_rt_err;
    }

    if (NULL == this.fnOutByte) {
        return fsm_rt_err;
    }

    while (this.wAmount) {
        if ((*this.fnOutByte)(this.pchData[this.wCount])) {
            this.wCount++;
            this.wAmount--;
        } else {
            return fsm_rt_on_going;
        }
    }

    return fsm_rt_cpl;
}


