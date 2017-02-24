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
DEF_CLASS(fifo_t)
    void *          pBuffer;
    fifo_uint_t     Size;       //!< fifo size.
    fifo_uint_t     Out;
    fifo_uint_t     In;
END_DEF_CLASS(fifo_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool fifo8_init(void *fifoObj, uint8_t *pBuffer, fifo_uint_t fifoSize)
{
    CLASS(fifo_t) *FIFO = (CLASS(fifo_t) *)fifoObj;
    
    if (NULL == fifoObj || NULL == pBuffer || (!IS_POWER_OF_2(fifoSize))) {
        return false;
    }

    FIFO->pBuffer = pBuffer;
    FIFO->Size    = fifoSize;
    FIFO->Out    = 0;
    FIFO->In    = 0;

    return true;
}

bool fifo8_in(void *fifoObj, const uint8_t *pBuffer)
{
    CLASS(fifo_t) *FIFO = (CLASS(fifo_t) *)fifoObj;
    fifo_uint_t L1;

    if (NULL == fifoObj || NULL == pBuffer) {
        return false;
    }

    L1 = FIFO->Size - (FIFO->In - FIFO->Out);
    if (L1 == 0u) {      //!< fifo is full.
        return false;
    }

    L1 = MIN(1u, L1);

    ((uint8_t *)FIFO->pBuffer)[FIFO->In & (FIFO->Size - 1u)] = *pBuffer;
    FIFO->In += L1;

    return true;
}

bool fifo8_out(void *fifoObj, uint8_t *pBuffer)
{
    CLASS(fifo_t) *FIFO = (CLASS(fifo_t) *)fifoObj;
    fifo_uint_t L1;

    if (NULL == fifoObj) {
        return false;
    }

    L1 = FIFO->In - FIFO->Out;
    if (L1 == 0u) {      //!< fifo is empty.
        return false;
    }

    L1 = MIN(1u, L1);

    if (NULL != pBuffer) {
        *pBuffer = ((uint8_t *)FIFO->pBuffer)[FIFO->Out & (FIFO->Size - 1u)];
    }
    FIFO->Out += L1;

    return true;
}

//bool fifo8_in_burst(void *fifoObj, const uint8_t *pBuffer, fifo_uint_t Size)
//{
//    CLASS(fifo_t) *FIFO = (CLASS(fifo_t) *)fifoObj;
//    fifo_uint_t L1, L2;
//
//    if (NULL == fifoObj || NULL == pBuffer) {
//        return false;
//    }
//
//    L1 = FIFO->Size - (FIFO->In - FIFO->Out);
//    L2 = FIFO->Size - (FIFO->In & (FIFO->Size - 1u));
//    if (L1 == 0u) {     //!< fifo is full.
//        return false;
//    }
//
//    L1 = MIN(Size, L1); //!< all those without data.
//    L2 = MIN(L1, L2);   //!< those can be access at once.
//
//    memcpy(FIFO->pBuffer + (FIFO->In & (FIFO->Size - 1u)), pBuffer, L2);
//    memcpy(FIFO->pBuffer, pBuffer + L2, L1 - L2);
//    FIFO->In += L1;
//
//    return true;
//}
//
//bool fifo8_out_burst(void *fifoObj, uint8_t *pBuffer, fifo_uint_t Size)
//{
//    CLASS(fifo_t) *FIFO = (CLASS(fifo_t) *)fifoObj;
//    fifo_uint_t L1, L2;
//
//    if (NULL == fifoObj) {
//        return false;
//    }
//
//    L1 = FIFO->In - FIFO->Out;
//    L2 = FIFO->Size - (FIFO->Out & (FIFO->Size - 1u));
//    if (L1 == 0u) {     //!< fifo is empty.
//        return false;
//    }
//
//    L1 = MIN(Size, L1); //!< all those with data.
//    L2 = MIN(L1, L2);   //!< those can be access at once.
//
//    if (NULL != pBuffer) {
//        memcpy(pBuffer, FIFO->pBuffer + (FIFO->Out & (FIFO->Size - 1u)), L2);
//        memcpy(pBuffer + L2, FIFO->pBuffer, L1 - L2);
//    }
//    FIFO->Out += L1;
//
//    return true;
//}

/* EOF */
