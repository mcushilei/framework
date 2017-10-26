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

#ifndef __USE_BITS_H__
#define __USE_BITS_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
//! \brief bit mask
#define BIT(__N)                (1u << (__N))
#define __BITMASK(__WIDTH)      (BIT(__WIDTH) - 1u)
#define BITMASK(__MSB, __LSB)   ((BIT(__MSB) | __BITMASK(__MSB)) ^ __BITMASK(__LSB))

/*============================ TYPES =========================================*/
typedef union {
    uint16_t    Value;
    uint8_t     Bytes[2];

    struct {
    #if defined(__BIG_ENDIAN__)
        uint8_t Byte1;
        uint8_t Byte0;
    #else
        uint8_t Byte0;
        uint8_t Byte1;
    #endif
    };
} hword_t;

typedef union {
    uint32_t    Value;
    uint16_t    HWords[2];
    uint8_t     Bytes[4];

    struct {
    #if defined(__BIG_ENDIAN__)
        uint16_t HWord1;
        uint16_t HWord0;
    #else
        uint16_t HWord0;
        uint16_t HWord1;
    #endif
    };

    struct {
    #if defined(__BIG_ENDIAN__)
        uint8_t Byte3;
        uint8_t Byte2;
        uint8_t Byte1;
        uint8_t Byte0;
    #else
        uint8_t Byte0;
        uint8_t Byte1;
        uint8_t Byte2;
        uint8_t Byte3;
    #endif
    };
} word_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
