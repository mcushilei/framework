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
#define FIFO8_INIT(__FIFO, __PBUF, __BUF_SIZE)                         \
    fifo8_init(&__FIFO, __PBUF, __BUF_SIZE)
#define FIFO8_IN(__FIFO, __OBJ)         fifo8_in(&__PFIFO, (const uint8_t *)&(__OBJ))
#define FIFO8_OUT(__FIFO, __OBJ)        fifo8_out(&__PFIFO, (uint8_t *)&(__POBJ))


/*============================ TYPES =========================================*/
EXTERN_CLASS(fifo_t)
    void *              pBuffer;
    __fifo_uint_t       fifoSize;       //!< fifo size.
    __fifo_uint_t       itemSize;       //!< fifo size.
    __fifo_uint_t       Out;
    __fifo_uint_t       In;
END_EXTERN_CLASS(fifo_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool fifo_init       (void *fifoObj, void *pBuffer, __fifo_uint_t fifoSize, __fifo_uint_t itemSize);
extern bool fifo_in         (void *fifoObj, const void *pBuffer);
extern bool fifo_out        (void *fifoObj, void *pBuffer);

extern bool fifo8_init      (void *fifoObj, uint8_t *pBuffer, __fifo_uint_t fifoSize);
extern bool fifo8_in        (void *fifoObj, const uint8_t *pBuffer);
extern bool fifo8_out       (void *fifoObj, uint8_t *pBuffer);
//extern bool fifo8_in_burst  (void *fifoObj, const uint8_t *pBuffer, __fifo_uint_t Size);
//extern bool fifo8_out_burst (void *fifoObj, uint8_t *pBuffer, __fifo_uint_t Size);

#endif
#endif
/* EOF */
