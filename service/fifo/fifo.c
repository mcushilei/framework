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
#include "..\string\string.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    void *              pBuffer;
    __fifo_uint_t       fifoSize;       //!< buffer size.
    __fifo_uint_t       itemSize;       //!< item size.
    __fifo_uint_t       Out;            //!< point to space filled.
    __fifo_uint_t       In;             //!< point to space empty.
} fifo_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool fifo_init(void *fifoObj, void *pBuffer, __fifo_uint_t fifoSize, __fifo_uint_t itemSize)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    
    if (NULL == FIFO || NULL == pBuffer || (!IS_POWER_OF_2(fifoSize))) {
        return false;
    }

    FIFO->pBuffer   = pBuffer;
    FIFO->itemSize  = itemSize;
    FIFO->fifoSize  = fifoSize;
    FIFO->Out       = 0u;
    FIFO->In        = 0u;

    return true;
}

bool fifo_in(void *fifoObj, const void *pBuffer)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    __fifo_uint_t L1;

    if (NULL == FIFO || NULL == pBuffer) {
        return false;
    }

    L1 = FIFO->fifoSize - (FIFO->In - FIFO->Out);
    if (L1 == 0u) {      //!< fifo is full.
        return false;
    }

    L1 = MIN(1u, L1);

    mem_copy((void *)((uint32_t)FIFO->pBuffer + (FIFO->In & (FIFO->fifoSize - 1u)) * FIFO->itemSize), pBuffer, FIFO->itemSize);
    FIFO->In += L1;

    return true;
}

bool fifo_out(void *fifoObj, void *pBuffer)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    __fifo_uint_t L1;

    if (NULL == FIFO) {
        return false;
    }

    L1 = FIFO->In - FIFO->Out;
    if (L1 == 0u) {      //!< fifo is empty.
        return false;
    }

    L1 = MIN(1u, L1);

    if (NULL != pBuffer) {
        mem_copy(pBuffer, (void *)((uint32_t)FIFO->pBuffer + (FIFO->Out & (FIFO->fifoSize - 1u)) * FIFO->itemSize), FIFO->itemSize);
    }
    FIFO->Out += L1;

    return true;
}

bool fifo8_init(void *fifoObj, uint8_t *pBuffer, __fifo_uint_t fifoSize)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    
    if (NULL == FIFO || NULL == pBuffer || (!IS_POWER_OF_2(fifoSize))) {
        return false;
    }

    FIFO->pBuffer   = pBuffer;
    FIFO->itemSize  = sizeof(uint8_t);
    FIFO->fifoSize  = fifoSize;
    FIFO->Out       = 0u;
    FIFO->In        = 0u;

    return true;
}

bool fifo8_in(void *fifoObj, const uint8_t *pBuffer)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    __fifo_uint_t L1;

    if (NULL == FIFO || NULL == pBuffer) {
        return false;
    }

    L1 = FIFO->fifoSize - (FIFO->In - FIFO->Out);
    if (L1 == 0u) {      //!< fifo is full.
        return false;
    }

    L1 = MIN(1u, L1);

    ((uint8_t *)FIFO->pBuffer)[FIFO->In & (FIFO->fifoSize - 1u)] = *pBuffer;
    FIFO->In += L1;

    return true;
}

bool fifo8_out(void *fifoObj, uint8_t *pBuffer)
{
    fifo_t *FIFO = (fifo_t *)fifoObj;
    __fifo_uint_t L1;

    if (NULL == FIFO) {
        return false;
    }

    L1 = FIFO->In - FIFO->Out;
    if (L1 == 0u) {      //!< fifo is empty.
        return false;
    }

    L1 = MIN(1u, L1);

    if (NULL != pBuffer) {
        *pBuffer = ((uint8_t *)FIFO->pBuffer)[FIFO->Out & (FIFO->fifoSize - 1u)];
    }
    FIFO->Out += L1;

    return true;
}

//__fifo_uint_t fifo8_in_burst(void *fifoObj, const uint8_t *pBuffer, __fifo_uint_t bufferSize)
//{
//    fifo_t *FIFO = (fifo_t *)fifoObj;
//    __fifo_uint_t L1, L2;
//
//    if (NULL == FIFO || NULL == pBuffer) {
//        return 0;
//    }
//
//    L1 = FIFO->fifoSize - (FIFO->In - FIFO->Out);
//    L2 = FIFO->fifoSize - (FIFO->In & (FIFO->fifoSize - 1u));
//    if (L1 == 0u) {     //!< fifo is full.
//        return 0;
//    }
//
//    L1 = MIN(bufferSize, L1); //!< all those without data, include back around
//    L2 = MIN(L1, L2);   //!< those can be access at once.
//
//    memcpy(FIFO->pBuffer + (FIFO->In & (FIFO->fifoSize - 1u)), pBuffer, L2);
//    memcpy(FIFO->pBuffer, pBuffer + L2, L1 - L2);
//    FIFO->In += L1;
//
//    return L1;
//}
//
//__fifo_uint_t fifo8_out_burst(void *fifoObj, uint8_t *pBuffer, __fifo_uint_t bufferSize)
//{
//    fifo_t *FIFO = (fifo_t *)fifoObj;
//    __fifo_uint_t L1, L2;
//
//    if (NULL == FIFO) {
//        return 0;
//    }
//
//    L1 = FIFO->In - FIFO->Out;
//    L2 = FIFO->fifoSize - (FIFO->Out & (FIFO->fifoSize - 1u));
//    if (L1 == 0u) {     //!< fifo is empty.
//        return 0;
//    }
//
//    L1 = MIN(bufferSize, L1); //!< all those with data, include back around
//    L2 = MIN(L1, L2);   //!< those can be access at once.
//
//    if (NULL != pBuffer) {
//        memcpy(pBuffer, FIFO->pBuffer + (FIFO->Out & (FIFO->fifoSize - 1u)), L2);
//        memcpy(pBuffer + L2, FIFO->pBuffer, L1 - L2);
//    }
//    FIFO->Out += L1;
//
//    return L1;
//}

/* EOF */
