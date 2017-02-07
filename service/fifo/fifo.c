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


//! \note do not move this pre-processor statement to other places
#define __FIFO_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(fifo8_t)
    uint8_t *       Buffer;
    __fifo_uint_t   Size;
    __fifo_uint_t   Out;
    __fifo_uint_t   In;
END_DEF_CLASS(fifo8_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool fifo8_init(fifo8_t *FIFOObj, uint8_t *Buffer, __fifo_uint_t Size)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    
    if (NULL == FIFOObj || NULL == Buffer || (!IS_POWER_OF_2(Size))) {
        return false;
    }

    FIFO->Buffer  = Buffer;
    FIFO->Size    = Size;
    FIFO->Out    = 0;
    FIFO->In    = 0;

    return true;
}

bool fifo8_in(fifo8_t *FIFOObj, const uint8_t *Buffer)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    __fifo_uint_t l1, l2;

    if (NULL == FIFOObj || NULL == Buffer) {
        return false;
    }

    l1 = FIFO->Size - (FIFO->In - FIFO->Out);
    l2 = FIFO->Size - (FIFO->In & (FIFO->Size - 1u));
    if (l1 == 0u) {      //!< fifo is full.
        return false;
    }

    l1 = MIN(1u, l1);
    l2 = MIN(l1, l2);

    FIFO->Buffer[FIFO->In & (FIFO->Size - 1u)] = *Buffer;
    FIFO->In += l1;

    return true;
}

bool fifo8_out(fifo8_t *FIFOObj, uint8_t *Buffer)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    __fifo_uint_t l1, l2;

    if (NULL == FIFOObj) {
        return false;
    }

    l1 = FIFO->In - FIFO->Out;
    l2 = FIFO->Size - (FIFO->Out & (FIFO->Size - 1u));
    if (l1 == 0u) {      //!< fifo is empty.
        return false;
    }

    l1 = MIN(1u, l1);
    l2 = MIN(l1, l2);

    if (NULL != Buffer) {
        *Buffer = FIFO->Buffer[FIFO->Out & (FIFO->Size - 1u)];
    }
    FIFO->Out += l1;

    return true;
}

bool fifo8_in_burst(fifo8_t *FIFOObj, const uint8_t *Buffer, __fifo_uint_t Size)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    __fifo_uint_t l1, l2;

    if (NULL == FIFOObj || NULL == Buffer) {
        return false;
    }

    l1 = FIFO->Size - (FIFO->In - FIFO->Out);
    l2 = FIFO->Size - (FIFO->In & (FIFO->Size - 1u));
    if (l1 == 0u) {     //!< fifo is full.
        return false;
    }

    l1 = MIN(Size, l1); //!< all those without data.
    l2 = MIN(l1, l2);   //!< those can be access at once.

    memcpy(FIFO->Buffer + (FIFO->In & (FIFO->Size - 1u)), Buffer, l2);
    memcpy(FIFO->Buffer, Buffer + l2, l1 - l2);
    FIFO->In += l1;

    return true;
}

bool fifo8_out_burst(fifo8_t *FIFOObj, uint8_t *Buffer, __fifo_uint_t Size)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    __fifo_uint_t l1, l2;

    if (NULL == FIFOObj) {
        return false;
    }

    l1 = FIFO->In - FIFO->Out;
    l2 = FIFO->Size - (FIFO->Out & (FIFO->Size - 1u));
    if (l1 == 0u) {     //!< fifo is empty.
        return false;
    }

    l1 = MIN(Size, l1); //!< all those with data.
    l2 = MIN(l1, l2);   //!< those can be access at once.

    if (NULL != Buffer) {
        memcpy(Buffer, FIFO->Buffer + (FIFO->Out & (FIFO->Size - 1u)), l2);
        memcpy(Buffer + l2, FIFO->Buffer, l1 - l2);
    }
    FIFO->Out += l1;

    return true;
}
/* EOF */
