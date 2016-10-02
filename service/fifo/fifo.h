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


#ifndef __FIFO_H__
#define __FIFO_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define FIFO_INIT(__PFIFO, __PBUF, __BUF_SIZE, __OBJ_SIZE)              \
    fifo_init(__PFIFO, __PBUF, __BUF_SIZE, __OBJ_SIZE)

#define FIFO_DEINIT(__PFIFO)            fifo_deinit(__PFIFO)

#define FIFO_UINT8_INIT(__PFIFO, __PBUF, __BUF_SIZE)                    \
    FIFO_INIT(__PFIFO, __PBUF, __BUF_SIZE, sizeof(uint8_t))

#define FIFO_UINT16_INIT(__PFIFO, __PBUF, __BUF_SIZE)                   \
    FIFO_INIT(__PFIFO, __PBUF, __BUF_SIZE, sizeof(uint16_t))

#define FIFO_UINT32_INIT(__PFIFO, __PBUF, __BUF_SIZE)                   \
    FIFO_INIT(__PFIFO, __PBUF, __BUF_SIZE, sizeof(uint32_t))

#define FIFO_IMPORT(__PFIFO, __OBJ)     fifo_import(__PFIFO, (void *)&(__OBJ))
#define FIFO_EXPORT(__PFIFO, __POBJ)    fifo_export(__PFIFO, (void *)(__POBJ))

/*============================ TYPES =========================================*/
EXTERN_CLASS(fifo_t)
    void *              pBuffer;
    __fifo_uint_t       tObjSize;   //!< in bytes
    __fifo_uint_t       tSize;
    __fifo_uint_t       tHead;
    __fifo_uint_t       tTail;
END_EXTERN_CLASS(fifo_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool fifo_init(fifo_t *pFIFO, void *pBuffer, __fifo_uint_t tSize, __fifo_uint_t tObjSize);
extern bool fifo_deinit(fifo_t *pFIFO);
extern bool fifo_import(fifo_t *pFIFO, void *ptObj);
extern bool fifo_export(fifo_t *pFIFO, void *ptObj);

#endif
