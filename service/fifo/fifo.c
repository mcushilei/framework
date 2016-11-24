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


/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(fifo8_t)
    uint8_t *   Buffer;
    uint32_t    Size;
    uint32_t    Out;
    uint32_t    In;
END_DEF_CLASS(fifo8_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
#if 0
bool fifo8_init(fifo8_t *FIFOObj, uint8_t *Buffer, uint32_t Size)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    
    if (NULL == FIFOObj || NULL == Buffer || Size < 2) {
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
    uint32_t l1, l2;

    if (NULL == FIFOObj || NULL == Buffer) {
        return false;
    }

    //! l1: all those without data.
    //! l2: those can be access at once.
    if (FIFO->Out > FIFO->In) {     //!< counter return.
        l2 = FIFO->Out - FIFO->In;
        l1 = l2;
    } else {
        l2 = FIFO->Size - FIFO->In;
        l1 = l2 + FIFO->Out;
    }
    if (l1 < 2) {   //!< fifo is full.
        return false;
    }

    l1 = MIN(1, l1);
    l2 = MIN(l1, l2);

    FIFO->Buffer[FIFO->In] = *Buffer;
    FIFO->In += l1;
    if (FIFO->In >= FIFO->Size) {
        FIFO->In = FIFO->Size - FIFO->In;
    }

    return true;
}

bool fifo8_out(fifo8_t *FIFOObj, uint8_t *Buffer)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    uint32_t l1, l2;

    if (NULL == FIFOObj) {
        return false;
    }

    //! l1: all those with data.
    //! l2: those can be access at once.
    if (FIFO->Out > FIFO->In) {    //!< counter return.
        l2 = FIFO->Size - FIFO->Out;
        l1 = l2 + FIFO->In;
    } else {
        l2 = FIFO->In - FIFO->Out;
        l1 = l2;
    }
    if (l1 == 0) {      //!< fifo is empty.
        return false;
    }

    l1 = MIN(1, l1);
    l2 = MIN(l1, l2);

    if (NULL != Buffer) {
        *Buffer = FIFO->Buffer[FIFO->Out];
    }
    FIFO->Out += l1;
    if (FIFO->Out >= FIFO->Size) {
        FIFO->Out = FIFO->Size - FIFO->Out;
    }

    return true;
}

#else

bool fifo8_init(fifo8_t *FIFOObj, uint8_t *Buffer, uint32_t Size)
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
    uint32_t l1, l2;

    if (NULL == FIFOObj || NULL == Buffer) {
        return false;
    }

    l1 = FIFO->Size - (FIFO->In - FIFO->Out);
    l2 = FIFO->Size - (FIFO->In & (FIFO->Size - 1));
    if (l1 == 0) {      //!< fifo is full.
        return false;
    }

    l1 = MIN(1, l1);
    l2 = MIN(l1, l2);

    FIFO->Buffer[FIFO->In & (FIFO->Size - 1)] = *Buffer;
    FIFO->In += l1;

    return true;
}

bool fifo8_out(fifo8_t *FIFOObj, uint8_t *Buffer)
{
    CLASS(fifo8_t) *FIFO = (CLASS(fifo8_t) *)FIFOObj;
    uint32_t l1, l2;

    if (NULL == FIFOObj) {
        return false;
    }

    l1 = FIFO->In - FIFO->Out;
    l2 = FIFO->Size - (FIFO->Out & (FIFO->Size - 1));
    if (l1 == 0) {      //!< fifo is empty.
        return false;
    }

    l1 = MIN(1, l1);
    l2 = MIN(l1, l2);

    if (NULL != Buffer) {
        *Buffer = FIFO->Buffer[FIFO->Out & (FIFO->Size - 1)];
    }
    FIFO->Out += l1;

    return true;
}

#endif

