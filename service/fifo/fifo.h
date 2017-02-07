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

#ifndef __FIFO_C__
#ifndef __FIFO_H__
#define __FIFO_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define FIFO8_INIT(__PFIFO, __PBUF, __BUF_SIZE)                         \
    fifo8_init(__PFIFO, __PBUF, __BUF_SIZE)
#define FIFO8_IN(__PFIFO, __OBJ)        fifo8_in(__PFIFO, (const uint8_t *)&(__OBJ))
#define FIFO8_OUT(__PFIFO, __POBJ)      fifo8_out(__PFIFO, (uint8_t *)(__POBJ))


/*============================ TYPES =========================================*/
EXTERN_CLASS(fifo8_t)
    uint8_t *       Buffer;
    __fifo_uint_t   Size;
    __fifo_uint_t   Out;
    __fifo_uint_t   In;
END_EXTERN_CLASS(fifo8_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool fifo8_init  (fifo8_t *FIFOObj, uint8_t *pBuffer, __fifo_uint_t Size);
extern bool fifo8_in    (fifo8_t *FIFOObj, const uint8_t *pBuffer);
extern bool fifo8_out   (fifo8_t *FIFOObj, uint8_t *pBuffer);
extern bool fifo8_in_burst  (fifo8_t *FIFOObj, const uint8_t *Buffer, __fifo_uint_t Size);
extern bool fifo8_out_burst (fifo8_t *FIFOObj, uint8_t *Buffer, __fifo_uint_t Size);

#endif
#endif
/* EOF */
