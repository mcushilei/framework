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


#ifndef __STRING_H__
#define __STRING_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include <string.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define CHAR_IS_NOT_HEX(__P)        ((__P) < '0'                                \
                                    || ((__P) > '9' && (__P) < 'A')             \
                                    || ((__P) > 'F' && (__P) < 'a')             \
                                    || (__P) > 'f')

#define CHAR_IS_NOT_INT(__CHAR)     ((__CHAR) < '0' || (__CHAR) > '9')
#define CHAR_IS_INT(__CHAR)         ((__CHAR) >= '0' && (__CHAR) <= '9')

#define CHAR_TO_INT(__CHAR)         ((__CHAR) - '0')
#define CHAR_IS_SPACE(__CHAR)       (' ' == (__CHAR))

#ifdef USE_STAND_STRING_LIB
#   define STRING_PRINTF(__string, __VA0, ...)          \
        printf(__string, __VA0, __VA_ARGS__)
#else
#   define STRING_PRINTF(__string, __VA0, ...)          \
        string_printf(__string, (uint32_t)__VA0)
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern bool     memory_compare(const void *m1, const void *m2, size_t n);
extern bool     memory_set(void *m, uint32_t v, size_t n);
extern bool     memory_copy(void *d, const void *s, size_t n);
extern uint32_t hex_str2uint(const uint8_t *str);
extern int32_t  int_str2int(const uint8_t *str);
extern float    dec_str2float(const uint8_t *decimalString);
extern size_t   string_length(const uint8_t *s);
extern uint8_t *string_string(const uint8_t *s1, const uint8_t *s2);
extern bool     string_cmp(const uint8_t *s1, const uint8_t *s2);
extern bool     string_cmpn(const uint8_t *s1, const uint8_t *s2, uint32_t n);
extern bool     string_copy(uint8_t *d, const uint8_t *s);
extern bool     string_copyn(uint8_t *d, const uint8_t *s, uint32_t n);
extern uint8_t *strsep(uint8_t **strngRef, const uint8_t *delim);
extern uint8_t *itostr(int32_t value, uint8_t *integerString, int32_t radix);
extern void     string_printf(const uint8_t *formatString, uint32_t argument);
extern uint32_t string_snprintf(uint8_t *buffer, uint32_t bufferLength, const uint8_t *formatString, uint32_t argument);

#endif
/* EOF */
