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

#ifndef __PIPE_C__
#ifndef __PIPE_H__
#define __PIPE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
EXTERN_CLASS(c_pipe_t, 
        typedef bool fn_output_byte_t(uint8_t chByte);
    )
    const uint8_t *pchData;
    fn_output_byte_t *fnOutByte;
    uint32_t      wAmount;
    uint32_t      wCount;
END_EXTERN_CLASS(c_pipe_t)

DEF_INTERFACE(i_pipe_t)
    bool     (*Init)(const uint8_t *pchData, uint32_t wAmount, fn_output_byte_t *fnOutByte, c_pipe_t *ptPipe);
    fsm_rt_t (*Out)(c_pipe_t *ptPipe);
END_DEF_INTERFACE(i_pipe_t)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_pipe_t PIPE;
/*============================ PROTOTYPES ====================================*/

#endif
#endif
