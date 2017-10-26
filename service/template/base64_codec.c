/*******************************************************************************
 *  Copyright(C)2017 by Dreistein<mcu_shilei@hotmail.com>                     *
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
#define __BASE64_CODEC_C__




/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\public.h"
#include "..\device.h"
#include ".\reg_template.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
uint32_t base64_encode(uint8_t in[], uint32_t inLen, uint8_t out[]) {
    static const uint8_t base64TBL[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    uint32_t i, j, n, outLen;
    uint8_t ta[4], ar[4];

    outLen = 0;
    for (i = 0; i < inLen;) {
        for (j = 0; j < 3 && i < inLen; j++) {
            ta[j] = in[i];
            i++;
        }
        switch (j) {
            case 1:
                n = 2;
                break;

            case 2:
                n = 3;
                break;

            default:
                n = 4;
                break;
        }
        for (; j < 3; j++) {
            ta[j] = 0;
        }
        ar[0] = base64TBL[          ta[0] >> 2                              ];
        ar[1] = base64TBL[(uint8_t)(ta[0] << 4) | ta[1] >> 4                ];
        ar[2] = base64TBL[(uint8_t)              (ta[1] << 2) | ta[2] >> 6  ];
        ar[3] = base64TBL[                                      ta[2]       ];
        for (j = 0; j < n; j++) {
            out[outLen] = ar[j];
            outLen++;
        }
        for (; j < 4; j++) {
            out[outLen] = '=';
            outLen++;
        }
    }

    return outLen;
}

/* EOF */
