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
#include "..\debug\debug.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DEF_CLASS(fifo_t)
    void *              pBuffer;
    __fifo_uint_t       tObjSize;
    __fifo_uint_t       tSize;
    __fifo_uint_t       tHead;
    __fifo_uint_t       tTail;
END_DEF_CLASS(fifo_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
DEBUG_DEFINE_THIS_FILE("FIFO");

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
bool fifo_init(fifo_t *pFIFO, void *pBuffer, __fifo_uint_t tSize, __fifo_uint_t tObjSize)
{
    CLASS(fifo_t) *ptFIFO = (CLASS(fifo_t) *)pFIFO;
    
    DEBUG_ASSERT_NOT_NULL(pFIFO);
    DEBUG_ASSERT_NOT_NULL(pBuffer);
    DEBUG_ASSERT(tSize >= 2, DEBUG_PRINT("FIFO Buffer size must not less than 2.", 0));
    if (NULL == pFIFO || NULL == pBuffer || tSize < 2) {
        return false;
    }

#if FIFO_TYPE_SIZE_ALIGNED == ENABLED
    DEBUG_ASSERT(IS_POWER_OF_2(tSize), DEBUG_PRINT("ERROR: FIFO Buffer size is not aligned."));
    if (!IS_POWER_OF_2(tSize)) {
        return false;
    }
#endif

    ptFIFO->pBuffer  = pBuffer;
    ptFIFO->tObjSize = tObjSize;
    ptFIFO->tSize    = tSize;
    ptFIFO->tHead    = 0;
    ptFIFO->tTail    = 0;

    return true;
}

bool fifo_deinit(fifo_t *pFIFO)
{
    CLASS(fifo_t) *ptFIFO = (CLASS(fifo_t) *)pFIFO;
    
    DEBUG_ASSERT_NOT_NULL(pFIFO);
    if (NULL == pFIFO) {
        return false;
    }

    ptFIFO->pBuffer  = NULL;
    ptFIFO->tObjSize = 0;
    ptFIFO->tSize    = 0;
    ptFIFO->tHead    = 0;
    ptFIFO->tTail    = 0;

    return true;
}

bool fifo_import(fifo_t *pFIFO, void *ptObj)
{
    CLASS(fifo_t) *ptFIFO = (CLASS(fifo_t) *)pFIFO;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pFIFO);
    if (NULL == pFIFO) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(ptFIFO->pBuffer);
    if (NULL == ptFIFO->pBuffer) {
        return false;
    }

    do {
        if (((ptFIFO->tTail + 1 >= ptFIFO->tSize)?
	      0 : ptFIFO->tTail + 1)
          ==  ptFIFO->tHead) {
            DEBUG_MSG(FIFO_DEBUG, "FIFO is full.");
            break;
        }

        switch (ptFIFO->tObjSize) {
            case 1:
                ((uint8_t *)(ptFIFO->pBuffer))[
                             ptFIFO->tTail] = *(uint8_t *)ptObj;
                break;
            case 2:
                ((uint16_t *)(ptFIFO->pBuffer))[
                              ptFIFO->tTail] = *(uint16_t *)ptObj;
                break;
            case 4:
                ((uint32_t *)(ptFIFO->pBuffer))[
                              ptFIFO->tTail] = *(uint32_t *)ptObj;
                break;
            default:
                mem_copy((uint8_t *)ptFIFO->pBuffer + (ptFIFO->tObjSize) * (ptFIFO->tTail),
                         ptObj,
                         ptFIFO->tObjSize);
                break;
        }
        ptFIFO->tTail++;
        if (ptFIFO->tTail >= ptFIFO->tSize) {
            ptFIFO->tTail = 0;
        }
        bResult = true;
    } while (false);

    return bResult;
}

bool fifo_export(fifo_t *pFIFO, void *ptObj)
{
    CLASS(fifo_t) *ptFIFO = (CLASS(fifo_t) *)pFIFO;
    bool bResult = false;

    DEBUG_ASSERT_NOT_NULL(pFIFO);
    if (NULL == pFIFO) {
        return false;
    }

    DEBUG_ASSERT_NOT_NULL(ptFIFO->pBuffer);
    if (NULL == ptFIFO->pBuffer) {
        return false;
    }

    do {
        if (ptFIFO->tHead == ptFIFO->tTail) {
            DEBUG_MSG(FIFO_DEBUG, "FIFO is empty.");
            break;
        }

        if (NULL != ptObj) {
            switch (ptFIFO->tObjSize) {
                case 1:
                    *(uint8_t *)ptObj = ((uint8_t *)(ptFIFO->pBuffer))[
                                                     ptFIFO->tHead];
                    break;
                case 2:
                    *(uint16_t *)ptObj = ((uint16_t *)(ptFIFO->pBuffer))[
                                                       ptFIFO->tHead];
                    break;
                case 4:
                    *(uint32_t *)ptObj = ((uint32_t *)(ptFIFO->pBuffer))[
                                                       ptFIFO->tHead];
                    break;
                default:
                    mem_copy(ptObj,
                             (uint8_t *)ptFIFO->pBuffer + (ptFIFO->tObjSize) * (ptFIFO->tHead),
                             ptFIFO->tObjSize);
                    break;
            }
        }
        ptFIFO->tHead++;
        if (ptFIFO->tHead >= ptFIFO->tSize) {
            ptFIFO->tHead = 0;
        }
        bResult = true;
    } while (false);

    return bResult;
}

