/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __USE_BITS_H__
#define __USE_BITS_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*! \brief exchange between big endian and little endian.
 *!        eg: some data in *p in BIG endian, use GET_U32_LE in LITTLE endian CPU
 *!        while get LITTLE edian word from *p. But, use GET_U32_LE in BIG
 *!        endian CPU will just get a copy in word from *p, so it's NO effect.
 */
#define GET_U16_BE(v, p)                                \
    do {                                                \
        uint16_t t;                                     \
        t = ( (uint16_t) ((uint8_t *)(p))[0] <<  8 )    \
          | ( (uint16_t) ((uint8_t *)(p))[1]       );   \
        (v) = t;                                        \
    } while (0)
        
#define GET_U24_BE(v, p)                                \
    do {                                                \
        uint32_t t;                                     \
        t = ( (uint32_t) ((uint8_t *)(p))[0] << 16 )    \
          | ( (uint32_t) ((uint8_t *)(p))[1] <<  8 )    \
          | ( (uint32_t) ((uint8_t *)(p))[2]       );   \
        (v) = t;                                        \
    } while (0)

#define GET_U32_BE(v, p)                                \
    do {                                                \
        uint32_t t;                                     \
        t = ( (uint32_t) ((uint8_t *)(p))[0] << 24 )    \
          | ( (uint32_t) ((uint8_t *)(p))[1] << 16 )    \
          | ( (uint32_t) ((uint8_t *)(p))[2] <<  8 )    \
          | ( (uint32_t) ((uint8_t *)(p))[3]       );   \
        (v) = t;                                        \
    } while (0)

#define GET_U16_LE(v, p)                                \
    do {                                                \
        uint16_t t;                                     \
        t = ( (uint16_t) ((uint8_t *)(p))[0]       )    \
          | ( (uint16_t) ((uint8_t *)(p))[1] <<  8 );   \
        (v) = t;                                        \
    } while (0)
        
#define GET_U24_LE(v, p)                                \
    do {                                                \
        uint32_t t;                                     \
        t = ( (uint32_t) ((uint8_t *)(p))[0]       )    \
          | ( (uint32_t) ((uint8_t *)(p))[1] <<  8 )    \
          | ( (uint32_t) ((uint8_t *)(p))[2] << 16 );   \
        (v) = t;                                        \
    } while (0)

#define GET_U32_LE(v, p)                                \
    do {                                                \
        uint32_t t;                                     \
        t = ( (uint32_t) ((uint8_t *)(p))[0]       )    \
          | ( (uint32_t) ((uint8_t *)(p))[1] <<  8 )    \
          | ( (uint32_t) ((uint8_t *)(p))[2] << 16 )    \
          | ( (uint32_t) ((uint8_t *)(p))[3] << 24 );   \
        (v) = t;                                        \
    } while (0)

#define SET_U16_BE(p, v)		                    \
    do {                                            \
        uint16_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t >>  8) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >>  0) & 0xFFu;    \
    } while (0)

#define SET_U24_BE(p, v)		                    \
    do {                                            \
        uint32_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t >> 16) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >>  8) & 0xFFu;    \
        ((uint8_t *)(p))[2] = (t >>  0) & 0xFFu;    \
    } while (0)

#define SET_U32_BE(p, v)		                    \
    do {                                            \
        uint32_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t >> 24) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >> 16) & 0xFFu;    \
        ((uint8_t *)(p))[2] = (t >>  8) & 0xFFu;    \
        ((uint8_t *)(p))[3] = (t      ) & 0xFFu;    \
    } while (0)

#define SET_U16_LE(p, v)		                    \
    do {                                            \
        uint16_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t      ) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >>  8) & 0xFFu;    \
    } while (0)

#define SET_U24_LE(p, v)		                    \
    do {                                            \
        uint32_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t      ) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >>  8) & 0xFFu;    \
        ((uint8_t *)(p))[2] = (t >> 16) & 0xFFu;    \
    } while (0)

#define SET_U32_LE(p, v)		                    \
    do {                                            \
        uint32_t t = (v);                           \
        ((uint8_t *)(p))[0] = (t      ) & 0xFFu;    \
        ((uint8_t *)(p))[1] = (t >>  8) & 0xFFu;    \
        ((uint8_t *)(p))[2] = (t >> 16) & 0xFFu;    \
        ((uint8_t *)(p))[3] = (t >> 24) & 0xFFu;    \
    } while (0)

#define GET_LE_U16(v, p)			GET_U16_LE(v, p)
#define GET_LE_U24(v, p)			GET_U24_LE(v, p)
#define GET_LE_U32(v, p)			GET_U32_LE(v, p)
#define GET_BE_U16(v, p)			GET_U16_BE(v, p)
#define GET_BE_U24(v, p)			GET_U24_BE(v, p)
#define GET_BE_U32(v, p)			GET_U32_BE(v, p)
#define SET_LE_U16(p, v)			SET_U16_LE(p, v)
#define SET_LE_U24(p, v)			SET_U24_LE(p, v)
#define SET_LE_U32(p, v)			SET_U32_LE(p, v)
#define SET_BE_U16(p, v)			SET_U16_BE(p, v)
#define SET_BE_U24(p, v)			SET_U24_BE(p, v)
#define SET_BE_U32(p, v)			SET_U32_BE(p, v)

#define SWAP_BYTE_U16(v)			((((uint16_t)(v) & 0xFF00u) >> 8) |         \
									 (((uint16_t)(v) & 0x00FFu) << 8))
#define SWAP_BYTE_U24(v)			((((uint32_t)(v) & 0x00FF0000u) >> 16) |    \
									 (((uint32_t)(v) & 0x0000FF00u) << 0)  |    \
									 (((uint32_t)(v) & 0x000000FFu) << 16))
#define SWAP_BYTE_U32(v)			((((uint32_t)(v) & 0xFF000000u) >> 24) |    \
									 (((uint32_t)(v) & 0x00FF0000u) >> 8)  |    \
									 (((uint32_t)(v) & 0x0000FF00u) << 8)  |    \
									 (((uint32_t)(v) & 0x000000FFu) << 24))

#if defined(__BIG_ENDIAN__) && (__BIG_ENDIAN__ == 1)
#	define LE_TO_SYS_U16(v)			SWAP_BYTE_U16(v)
#	define LE_TO_SYS_U24(v)			SWAP_BYTE_U24(v)
#	define LE_TO_SYS_U32(v)			SWAP_BYTE_U32(v)
#	define BE_TO_SYS_U16(v)			((uint16_t)(v))
#	define BE_TO_SYS_U24(v)			((uint32_t)(v))
#	define BE_TO_SYS_U32(v)			((uint32_t)(v))

#	define SYS_TO_LE_U16(v)			SWAP_BYTE_U16(v)
#	define SYS_TO_LE_U24(v)			SWAP_BYTE_U24(v)
#	define SYS_TO_LE_U32(v)			SWAP_BYTE_U32(v)
#	define SYS_TO_BE_U16(v)			((uint16_t)(v))
#	define SYS_TO_BE_U24(v)			((uint32_t)(v))
#	define SYS_TO_BE_U32(v)			((uint32_t)(v))

#	define GET_SYS_U16(v, p)		GET_BE_U16(v, p)
#	define GET_SYS_U24(v, p)		GET_BE_U24(v, p)
#	define GET_SYS_U32(v, p)		GET_BE_U32(v, p)

#else
#	define LE_TO_SYS_U16(v)			((uint16_t)(v))
#	define LE_TO_SYS_U24(v)			((uint32_t)(v))
#	define LE_TO_SYS_U32(v)			((uint32_t)(v))
#	define BE_TO_SYS_U16(v)			SWAP_BYTE_U16(v)
#	define BE_TO_SYS_U24(v)			SWAP_BYTE_U24(v)
#	define BE_TO_SYS_U32(v)			SWAP_BYTE_U32(v)

#	define SYS_TO_LE_U16(v)			((uint16_t)(v))
#	define SYS_TO_LE_U24(v)			((uint32_t)(v))
#	define SYS_TO_LE_U32(v)			((uint32_t)(v))
#	define SYS_TO_BE_U16(v)			SWAP_BYTE_U16(v)
#	define SYS_TO_BE_U24(v)			SWAP_BYTE_U24(v)
#	define SYS_TO_BE_U32(v)			SWAP_BYTE_U32(v)

#	define GET_SYS_U16(v, p)		GET_LE_U16(v, p)
#	define GET_SYS_U24(v, p)		GET_LE_U24(v, p)
#	define GET_SYS_U32(v, p)		GET_LE_U32(v, p)
#endif     

//! \brief bit mask
#define BIT(__N)            (1u << (__N))
#define __MASK(__N)         (BIT(__N) - 1u)
#define MASK(__MSB, __LSB)  ((BIT(__MSB) | __MASK(__MSB)) ^ __MASK(__LSB))

/*============================ TYPES =========================================*/
typedef union {
    uint16_t    Value;
    uint8_t     Bytes[2];

    struct {
    #if ((!defined(__BIG_ENDIAN__)) || (!__BIG_ENDIAN__))
        uint8_t Byte0;
        uint8_t Byte1;
    #else
        uint8_t Byte1;
        uint8_t Byte0;
    #endif
    };
} hword_t;

typedef union {
    uint32_t    Value;
    uint16_t    HWords[2];
    uint8_t     Bytes[4];

    struct {
    #if ((!defined(__BIG_ENDIAN__)) || (!__BIG_ENDIAN__))
        uint16_t HWord0;
        uint16_t HWord1;
    #else
        uint16_t HWord1;
        uint16_t HWord0;
    #endif
    };

    struct {
    #if ((!defined(__BIG_ENDIAN__)) || (!__BIG_ENDIAN__))
        uint8_t Byte0;
        uint8_t Byte1;
        uint8_t Byte2;
        uint8_t Byte3;
    #else
        uint8_t Byte3;
        uint8_t Byte2;
        uint8_t Byte1;
        uint8_t Byte0;
    #endif
    };
} word_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
