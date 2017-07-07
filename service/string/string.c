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
#define CHAR_IS_NOT_HEX(__P)        ((__P) < '0'                                \
                                    || ((__P) > '9' && (__P) < 'A')             \
                                    || ((__P) > 'F' && (__P) < 'a')             \
                                    || (__P) > 'f')

#define CHAR_IS_NOT_INT(__CHAR)     ((__CHAR) < '0' || (__CHAR) > '9')

#define CHAR_TO_INT(__CHAR)         ((__CHAR) - '0')
#define CHAR_IS_SPACE(__CHAR)       (' ' == (__CHAR))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
bool memory_compare(const void *m1, const void *m2, size_t n)
{
    if ((NULL == m1) || (NULL == m2) || (0 == n)) {
        return false;
    }

    for (; n; --n) {
        if (*(uint8_t *)m1 != *(uint8_t *)m2) {
            return true;
        }
        m1 = (uint8_t *)m1 + 1;
        m2 = (uint8_t *)m2 + 1;
    }

    return false;
}

bool memory_copy(void *d, const void *s, size_t n)
{
    for (; n; --n) {
        *(uint8_t *)d = *(uint8_t *)s;
        d = (uint8_t*)d + 1;
        s = (uint8_t*)s + 1;
    }

    return true;
}

//! this function is not transplantable.
#if defined(__CPU_ARM__) || defined(__CPU_X86__)
bool memory_set(void *m, uint32_t v, size_t n)
{
    if (n < 20) {
        uint8_t *p = (uint8_t *)m;
        for (; n; --n) {
            *p = v;
            p++;
        }
    } else {
        uint32_t i;

        v = v & 0xFFu;
        v = v | (v << 8) | (v << 16) | (v << 24);

        //! fill prefix-gap.
        i = 4u - (((uint32_t)m) & 0x03u);
        if (i > n) {
            i = n;
        }
        if (i & 0x03u) {
            uint8_t *p = (uint8_t *)m;
            m = (uint8_t *)m + i;
            n -= i;
            for (; i; --i) {
                *p = v;
                p++;
            }
        }

        //! fill word.
        i = n >> 2;
        if (i) {
            uint32_t *p = (uint32_t *)m;
            m = (uint32_t *)m + i;
            n &= 0x03u;
            for (; i; --i) {
                *p = v;
                p++;
            }
        } while (0);

        //! fill surfix-gap.
        if (n) {
            uint8_t *p = (uint8_t *)m;
            for (; n; --n) {
                *p = v;
                p++;
            }
        }
    }

    return true;
}
#else
bool memory_set(void *m, uint32_t v, size_t n)
{
    memset(m, v, n);
    return true;
}
#endif

uint32_t hex_str2uint(const uint8_t *str)
{
    uint32_t i, val = 0;

    if (NULL == str) {
        return 0;
    }

    for (i = 0; i < 8; i++) {
        if (CHAR_IS_NOT_HEX(*str)) {
            break;
        }
        val = (val << 4) + ((*str > '9')? ((*str < 'a')? (10 + (*str - 'A')) : (10 + (*str - 'a'))) : (*str - '0')) ;
        ++str;
    }

    return val;
}

int32_t int_str2int(const uint8_t *str)
{
    int32_t val = 0;
    uint32_t sign = 0;

    if (NULL == str) {
        return 0;
    }

    for (; CHAR_IS_SPACE(*str); ++str);

    if ('-' == *str) {
        sign = 1;
    }

    if ('-' == *str || '+' == *str) {
        ++str;
    }

    for (; !CHAR_IS_NOT_INT(*str); str++) {
        val *= 10;
        val += CHAR_TO_INT(*str);
    }

    if (sign) {
        return -val;
    }
    return val;
}

float dec_str2float(const uint8_t *DecimalString)
{ 
    bool IsNegative = false; 
    bool IsDecimal = false;
    uint8_t ByteChar = '0'; 
    float Integer = 0.0f; 
    float Decimal = 0.0f; 
    float DecPower = 0.1f; 

    if (NULL == DecimalString) { 
        return 0.0; 
    } 

    if (DecimalString[0] == '-') { 
        IsNegative = true; 
        DecimalString++; 
    } else { 
        IsNegative = false; 
    } 
 
    for (; *DecimalString != '\0'; ++DecimalString) {
        ByteChar = *DecimalString;

        if (CHAR_IS_NOT_INT(ByteChar)) { 
            if (ByteChar == '.') { 
                IsDecimal = true;
                ++DecimalString;
            }
            break; 
        } else {
            Integer = Integer * 10.0f + CHAR_TO_INT(ByteChar);
        }
    } 

    if (IsDecimal) {
        for (; *DecimalString != '\0'; ++DecimalString) {
            ByteChar = *DecimalString;

            if (CHAR_IS_NOT_INT(ByteChar)) {
                break;
            } else {
                Decimal += CHAR_TO_INT(ByteChar) * DecPower; 
                DecPower = DecPower * 0.1f; 
            }
        } 
    }

    return (IsNegative? -(Integer + Decimal) : Integer + Decimal); 
} 

size_t string_length(const uint8_t *s)
{
    size_t len = 0;

    for (; *s != '\0'; s++) {
        len++;
    }

    return len;
}

bool string_cmp(const uint8_t *s1, const uint8_t *s2)
{
    if ((*s1 == '\0') && (*s2 == '\0')) {
        return false;
    }
    
    if (*s1 && *s2) {
        while (*s1 && *s2) {
            if (*s1++ != *s2++) {
                return true;
            }
        }
    } else {
        return true;
    }

    return false;
}

bool string_cmpn(const uint8_t *s1, const uint8_t *s2, uint32_t n)
{
    if ((NULL == s1) || (NULL == s2) || (0 == n)) {
        return false;
    }

    for (; n; --n) {
        if (*s1++ != *s2++) {
            return true;
        }
    }

    return false;
}

uint8_t *string_copy(uint8_t *d, const uint8_t *s)
{
    if ((NULL == d) || (NULL == s)) {
        return d;
    }

    while ('\0' != *s) {
        *d++ = *s++;
    }
    *d = *s;

    return d;
}

//! find s2 in s1.
uint8_t *string_string(const uint8_t *s1, const uint8_t *s2)
{
    uint32_t n;
    
    if (*s2) {
        for (; *s1; s1++) {
            for (n = 0; s1[n] == s2[n]; n++) {
                if (!s2[n + 1]) {
                    return (uint8_t *)s1;
                }
            }
        }
    } else {
        for (; *s1; s1++);
        return (uint8_t *)s1;
    }
    
    return NULL;
}

/*! \brief  Paser string to token, replace delim to '\0' and return point to token.
 *!         String should be end with '\0'.
 *! \param ppStringRef address of point varible point to string that to paser.
 *! \param pDelim point to delim string, every char in this sting represent as a delim.
 *! \retval address of token, this token may be empty string:"\0".
 *! \retval NULL there is no token in specify string.
 */
uint8_t *strsep(uint8_t **ppStringRef, const uint8_t *pDelim)
{
	uint8_t *s, *tok;
	const uint8_t *spanp;
	uint8_t c, sc;

    s = *ppStringRef;
    if (s == NULL) {
        return NULL;
    }

    tok = s;
	for (;;) {
        c = *s;
        if (c == '\0') {
            *ppStringRef = NULL;
            return tok;
        } else {
		    for (spanp = pDelim; *spanp != '\0'; spanp++) {
                sc = *spanp;
			    if (sc == c) {
					*s = '\0';
				    *ppStringRef = s + 1;
                    return tok;
			    }
		    }
        }
        s++;
	}
}


uint8_t *itostr(int32_t Value, uint8_t *IntegerString, int32_t Radix)
{
    const static unsigned char c[16] = "0123456789ABCDEF";
    uint32_t wValue = Value;

    if (Radix < 0) {
        Radix = -Radix;

        if (Value < 0) {
            wValue = -Value;
        }
    }

    if ((NULL == IntegerString)
    || ((10 != Radix) && (16 != Radix))) {
        return NULL;
    }

    if (0 == wValue) {
        *IntegerString++ = c[0];
        *IntegerString = '\0';
    } else {
        uint32_t i = 0;
        if (10 == Radix) {         // decimal
            uint32_t tmp;
            const static uint32_t muntable[10] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};
            for (i = 0; i < 10; ++i) {
                tmp = wValue / muntable[i];
                if (tmp) {
                    break;
                }
            }
            for (; i < 10; ++i) {
                tmp = wValue / muntable[i];
                *IntegerString++ = c[tmp];
                wValue -= tmp * muntable[i];
            }
        } else {                    // hex
            for (i = 7; i; --i) {
                if ((wValue >> (i << 2)) & 0x0000000Fu) {
                    break;
                }
            }
            for (; i; --i) {
                *IntegerString++ = c[(wValue >> (i << 2)) & 0x0000000Fu];
            }
            *IntegerString++ = c[wValue & 0x0000000Fu];
        }
        *IntegerString = '\0';
    }

    return IntegerString;
}


#define PRINT(__PFORMAT, __PRINT_OUT, __ARG)                                    \
    for (; ; ) {                                                                \
        if ('\0' == *__PFORMAT) {                                               \
            break;                                                              \
        } else if ('%' == *__PFORMAT) {                                         \
            uint8_t buf[11];                                                    \
            uint8_t fill = ' ';                                                 \
            uint32_t i = 0, pl = 0;                                             \
                                                                                \
            __PFORMAT++;                                                        \
            if (*__PFORMAT >= '0' && *__PFORMAT <= '9') {                       \
                if (*__PFORMAT == '0') {                                        \
                    fill = '0';                                                 \
                    __PFORMAT++;                                                \
                }                                                               \
                                                                                \
                while (*__PFORMAT >= '0' && *__PFORMAT <= '9') {                \
                    pl = pl * 10 + (*__PFORMAT - '0');                          \
                    ++__PFORMAT;                                                \
                }                                                               \
            }                                                                   \
            if ('\0' == *__PFORMAT) {                                           \
                break;                                                          \
            }                                                                   \
                                                                                \
            switch (*__PFORMAT) {                                               \
                case 'c':                                                       \
                {                                                               \
                    uint8_t c = __ARG(uint8_t);                                 \
                    __PRINT_OUT(c);                                             \
                    break;                                                      \
                }                                                               \
                case 's':                                                       \
                {                                                               \
                    const uint8_t *s = __ARG(const uint8_t *);                  \
                    if (NULL == s) {                                            \
                        break;                                                  \
                    }                                                           \
                                                                                \
                    i = string_length(s);                                          \
                    if (pl < i) {                                               \
                        pl = i;                                                 \
                    }                                                           \
                                                                                \
                    pl -= i;                                                    \
                    for (i = 0; i < pl; ++i) {                                  \
                        __PRINT_OUT(fill);                                      \
                    }                                                           \
                                                                                \
                    for (; *s; s++) {                                           \
                        __PRINT_OUT(*s);                                        \
                    }                                                           \
                    break;                                                      \
                }                                                               \
                case 'u':                                                       \
                {                                                               \
                    uint32_t d = __ARG(uint32_t);                               \
                    itostr(d, buf, 10);                                         \
                                                                                \
                    i = string_length(buf);                                        \
                    if (pl < i) {                                               \
                        pl = i;                                                 \
                    }                                                           \
                                                                                \
                    pl -= i;                                                    \
                    for (i = 0; i < pl; ++i) {                                  \
                        __PRINT_OUT(fill);                                      \
                    }                                                           \
                                                                                \
                    for (i = 0; buf[i]; ++i) {                                  \
                        __PRINT_OUT(buf[i]);                                    \
                    }                                                           \
                    break;                                                      \
                }                                                               \
                case 'i':                                                       \
                case 'd':                                                       \
                {                                                               \
                    int32_t n = __ARG(int32_t);                                 \
                    itostr(n, buf, -10);                                        \
                                                                                \
                    i = string_length(buf);                                        \
                    if (n < 0) {                                                \
                        i += 1;                                                 \
                    }                                                           \
                    if (pl < i) {                                               \
                        pl = i;                                                 \
                    }                                                           \
                                                                                \
                    pl -= i;                                                    \
                    if (n >= 0) {                                               \
                        for (i = 0; i < pl; ++i) {                              \
                            __PRINT_OUT(fill);                                  \
                        }                                                       \
                    } else {                                                    \
                        if (fill == '0') {                                      \
                            __PRINT_OUT('-');                                   \
                            for (i = 0; i < pl; ++i) {                          \
                                __PRINT_OUT('0');                               \
                            }                                                   \
                        } else {                                                \
                            for (i = 0; i < pl; ++i) {                          \
                                __PRINT_OUT(' ');                               \
                            }                                                   \
                            __PRINT_OUT('-');                                   \
                        }                                                       \
                    }                                                           \
                                                                                \
                    for (i = 0; buf[i]; ++i) {                                  \
                        __PRINT_OUT(buf[i]);                                    \
                    }                                                           \
                    break;                                                      \
                }                                                               \
                case 'x':                                                       \
                case 'X':                                                       \
                {                                                               \
                    uint32_t d = __ARG(uint32_t);                               \
                    itostr(d, buf, 16);                                         \
                                                                                \
                    i = string_length(buf);                                        \
                    if (pl < i) {                                               \
                        pl = i;                                                 \
                    }                                                           \
                                                                                \
                    pl -= i;                                                    \
                    for (i = 0; i < pl; ++i) {                                  \
                        __PRINT_OUT(fill);                                      \
                    }                                                           \
                                                                                \
                    if ('X' == *__PFORMAT) {                                    \
                        for (i = 0; buf[i]; ++i) {                              \
                            __PRINT_OUT(buf[i]);                                \
                        }                                                       \
                    } else {                                                    \
                        for (i = 0; buf[i]; ++i) {                              \
                            if (buf[i] > '@') {                                 \
                                buf[i] = ' ' + buf[i];                          \
                            }                                                   \
                            __PRINT_OUT(buf[i]);                                \
                        }                                                       \
                    }                                                           \
                    break;                                                      \
                }                                                               \
                case 'p':                                                       \
                {                                                               \
                    uint32_t d = __ARG(uint32_t);                               \
                    itostr(d, buf, 16);                                         \
                                                                                \
                    if (pl < (sizeof(uint32_t) * 2)) {                          \
                        pl = (sizeof(uint32_t) * 2);                            \
                    }                                                           \
                    pl -= (sizeof(uint32_t) * 2);                               \
                    for (i = 0; i < pl; ++i) {                                  \
                        __PRINT_OUT(' ');                                       \
                    }                                                           \
                                                                                \
                    i = string_length(buf);                                        \
                    pl = (sizeof(uint32_t) * 2) - i;                            \
                    for (i = 0; i < pl; i++) {                                  \
                        __PRINT_OUT('0');                                       \
                    }                                                           \
                                                                                \
                    for (i = 0; buf[i]; ++i) {                                  \
                        __PRINT_OUT(buf[i]);                                    \
                    }                                                           \
                    break;                                                      \
                }                                                               \
                default:                                                        \
                    __PRINT_OUT(*__PFORMAT);                                    \
                    break;                                                      \
            }                                                                   \
        } else {                                                                \
            __PRINT_OUT(*__PFORMAT);                                            \
        }                                                                       \
        __PFORMAT++;                                                            \
    }                                                                           \


extern int user_printf_output_char(char cChar);
#define USER_PRINTF_OUT(__C)        user_printf_output_char(__C)
#define USER_PRINTF_ARG(__T)        (__T)Argument

void string_printf(const uint8_t *FormatString, uint32_t Argument)
{
    PRINT(FormatString, USER_PRINTF_OUT, USER_PRINTF_ARG)
}

#define USER_SPRINTFN_OUT(__C)                                                  \
    do {                                                                        \
        if (wCnt >= BufferLength) {                                                     \
            Buffer[wCnt] = '\0';                                         \
            return wCnt;                                                        \
        }                                                                       \
                                                                                \
        Buffer[wCnt] = __C;                                              \
        wCnt++;                                                                 \
    } while (0)

uint32_t string_snprintf(uint8_t *Buffer, uint32_t BufferLength, const uint8_t *FormatString, uint32_t Argument)
{
    uint32_t wCnt = 0;

    if ((NULL == Buffer) || (0 == BufferLength)) {
        return 0;
    }

    BufferLength--;
    PRINT(FormatString, USER_SPRINTFN_OUT, USER_PRINTF_ARG)
    Buffer[wCnt] = '\0';

    return wCnt;
}


/* EOF */
