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
#define __DEBUG_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#ifdef __DEBUG__

#if DEBUG_FOMART_STRING == ENABLED
#include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#if DEBUG_FOMART_STRING == ENABLED
    #define __DEBUG_PRINT(...)          printf(__VA_ARGS__);
#else
    #define __DEBUG_PRINT(string, ...)  debug_print_string(string);
#endif

#define DEBUG_PRINT(...)                __DEBUG_PRINT(__VA_ARGS__)

#define DEBUG_PRINT_EOL {                            \
    DEBUG_OUTPUT_CHAR('\r');                         \
    DEBUG_OUTPUT_CHAR('\n');                         \
}

#define DEBUG_PRINT_LOCATION(__FILE, __LINE) {       \
    DEBUG_PRINT(__FILE);                             \
    DEBUG_OUTPUT_CHAR('-');                          \
    DEBUG_OUTPUT_CHAR('>');                          \
    debug_print_number_unsigned(__LINE);             \
    DEBUG_OUTPUT_CHAR(':');                          \
    DEBUG_OUTPUT_CHAR(' ');                          \
}

/*============================ TYPES =========================================*/
//-------------------------------------------------------
// Internal Structs Needed
//-------------------------------------------------------
typedef enum {
    DEBUG_DISPLAY_STYLE_INT      = 0,
    DEBUG_DISPLAY_STYLE_UINT,
    DEBUG_DISPLAY_STYLE_HEX,
    DEBUG_DISPLAY_STYLE_POINTER,
} em_DebugDisplayStyle_t;

/*============================ PROTOTYPES ====================================*/
static void debug_print_number_signed(const _U_SINT number);
static void debug_print_number_unsigned(const _U_UINT number);
static void debug_print_number_hex(const _U_UINT number, const _U_UINT nibbles_to_print);
static void debug_print_mask(const _U_UINT mask, const _U_UINT number);

/*============================ LOCAL VARIABLES ===============================*/
static DEBUG_ROM_VAR_TYPE const char DebugStrFail[]      = "Failure: ";
static DEBUG_ROM_VAR_TYPE const char DebugStrMessage[]   = "Message: ";
static DEBUG_ROM_VAR_TYPE const char DebugStrNull[]      = "NULL";
static DEBUG_ROM_VAR_TYPE const char DebugStrExpected[]  = "Expected ";
static DEBUG_ROM_VAR_TYPE const char DebugStrWas[]       = " Was ";
static DEBUG_ROM_VAR_TYPE const char DebugStrNullPointer[]   = "Pointer was NULL.";

static volatile unsigned char s_chExitTrap = 0; 

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
void debug_trap(void)
{
    //__disable_interrupts();
    while (!s_chExitTrap) {
    }

    s_chExitTrap = 0;
}

void debug_exit_trap(void)
{
    s_chExitTrap = 1;
}

int debug_string_compare(const char *expected, const char *actual)
{
    if (expected && actual) {
        for (; *expected || *actual;) {
            if (*expected++ != *actual++) {
                return 0;
            }
        }
    } else if (expected || actual) {
        return 0;
    }

    return 1;
}

//-----------------------------------------------
// Pretty Printers
//-----------------------------------------------
#if DEBUG_FOMART_STRING == ENABLED
int putchar(int ch)
{
    DEBUG_OUTPUT_CHAR(ch);
 
    return ch;
}
#else
void debug_print_string(const char *string)
{
    if (string != NULL) {
        for (; *string; string++) {
            DEBUG_OUTPUT_CHAR(*string);
        }
    }
}
#endif

//-----------------------------------------------
//! basically do an itoa using as little ram as possible
static void debug_print_number_signed(const _U_SINT number_to_print)
{
    _U_SINT divisor = 1;
    _U_SINT next_divisor;
    _U_SINT number = number_to_print;

    if (number < 0) {
        DEBUG_OUTPUT_CHAR('-');
        number = -number;
    }

    // figure out initial divisor
    while (number / divisor > 9) {
        next_divisor = divisor * 10;
        if (next_divisor > divisor) {
            divisor = next_divisor;
        } else {
            break;
        }
    }

    // now mod and print, then divide divisor
    do {
        DEBUG_OUTPUT_CHAR((char)('0' + (number / divisor % 10)));
        divisor /= 10;
    } while (divisor > 0);
}

//-----------------------------------------------
//! basically do an itoa using as little ram as possible
static void debug_print_number_unsigned(const _U_UINT number)
{
    _U_UINT divisor = 1;
    _U_UINT next_divisor;

    // figure out initial divisor
    while (number / divisor > 9) {
        next_divisor = divisor * 10;
        if (next_divisor > divisor) {
            divisor = next_divisor;
        } else {
            break;
        }
    }

    // now mod and print, then divide divisor
    do {
        DEBUG_OUTPUT_CHAR((char)('0' + (number / divisor % 10)));
        divisor /= 10;
    } while (divisor > 0);
}

//-----------------------------------------------
static void debug_print_number_hex(const _U_UINT number, const _U_UINT nibbles_to_print)
{
    _U_UINT nibble;
    _U_UINT nibbles = nibbles_to_print;

    if (nibbles > (sizeof(_U_UINT) << 1)) {
        nibbles = sizeof(_U_UINT) << 1;
    }

    for (; nibbles; --nibbles) {
        nibble = (number >> ((nibbles - 1) << 2)) & 0x0Fu;
        if (nibble <= 9) {
            DEBUG_OUTPUT_CHAR((char)('0' + nibble));
        } else {
            DEBUG_OUTPUT_CHAR((char)('A' - 10 + nibble));
        }
    }
}

//-----------------------------------------------
static void debug_print_number_by_style(const _U_SINT number, const em_DebugDisplayStyle_t style)
{
    switch (style) {
        case DEBUG_DISPLAY_STYLE_INT:
            debug_print_number_signed((_U_SINT)number);
            break;

        case DEBUG_DISPLAY_STYLE_UINT:
            debug_print_number_unsigned((_U_UINT)number);
            break;

        case DEBUG_DISPLAY_STYLE_HEX:
        case DEBUG_DISPLAY_STYLE_POINTER:
        default:
            debug_print_number_hex((_U_UINT)number, 2 * sizeof(_U_UINT));
            break;
    }
}

//-----------------------------------------------
static void debug_print_mask(const _U_UINT mask, const _U_UINT number)
{
    _U_UINT current_bit = (_U_UINT)1 << (DEBUG_INT_WIDTH - 1);
    _U_UINT i;

    for (i = 0; i < DEBUG_INT_WIDTH; i++) {
        if (current_bit & mask) {
            if (current_bit & number) {
                DEBUG_OUTPUT_CHAR('1');
            } else {
                DEBUG_OUTPUT_CHAR('0');
            }
        } else {
            DEBUG_OUTPUT_CHAR('X');
        }
        current_bit = current_bit >> 1;
    }
}

//-----------------------------------------------
// Debug Result Output Handlers
//-----------------------------------------------
//-----------------------------------------------
void debug_print_null_point(void)
{
    DEBUG_PRINT(DebugStrNullPointer);
}

//-----------------------------------------------
void debug_print_expected_actual_string(const char *expected, const char *actual)
{
    DEBUG_PRINT(DebugStrExpected);
    if (expected != NULL) {
        DEBUG_OUTPUT_CHAR('\"');
        DEBUG_PRINT(expected);
        DEBUG_OUTPUT_CHAR('\"');
    } else {
      DEBUG_PRINT(DebugStrNull);          
    }

    DEBUG_PRINT(DebugStrWas);
    if (actual != NULL) {
        DEBUG_OUTPUT_CHAR('\"');
        DEBUG_PRINT(actual);
        DEBUG_OUTPUT_CHAR('\"');
    } else {
      DEBUG_PRINT(DebugStrNull);          
    }
}

//-----------------------------------------------
void debug_print_equal_bits(
    const _U_UINT mask,
    const _U_UINT expected,
    const _U_UINT actual)
{
    DEBUG_PRINT(DebugStrExpected);
    debug_print_mask(mask, expected);
    DEBUG_PRINT(DebugStrWas);
    debug_print_mask(mask, actual);
}

//-----------------------------------------------
void debug_print_equal_number(
    const _U_SINT expected, 
    const _U_SINT actual, 
    const em_DebugDisplayStyle_t style)
{
    DEBUG_PRINT(DebugStrExpected);
    switch (style) {
        case DEBUG_DISPLAY_STYLE_INT:
            debug_print_number_signed(expected);
            break;

        case DEBUG_DISPLAY_STYLE_UINT:
            debug_print_number_unsigned(expected);
            break;

        case DEBUG_DISPLAY_STYLE_HEX:
        case DEBUG_DISPLAY_STYLE_POINTER:
        default:
            debug_print_number_hex(expected, 2 * sizeof(_U_UINT));
            break;
    }

    DEBUG_PRINT(DebugStrWas);
    switch (style) {
        case DEBUG_DISPLAY_STYLE_INT:
            debug_print_number_signed(actual);
            break;

        case DEBUG_DISPLAY_STYLE_UINT:
            debug_print_number_unsigned(actual);
            break;

        case DEBUG_DISPLAY_STYLE_HEX:
        case DEBUG_DISPLAY_STYLE_POINTER:
        default:
            debug_print_number_hex(actual, 2 * sizeof(_U_UINT));
            break;
    }
}

//-----------------------------------------------
// Control Functions
//-----------------------------------------------
void debug_failure_captured(const char *file, const _U_UINT line)
{
    DEBUG_PRINT_EOL
    DEBUG_PRINT_LOCATION(file, line)
    DEBUG_PRINT(DebugStrFail);
}

void debug_msg_output(const char *file, const _U_UINT line)
{
    DEBUG_PRINT_EOL
    DEBUG_PRINT_LOCATION(file, line)
    DEBUG_PRINT(DebugStrMessage);
}

#endif      /* #ifdef __DEBUG__ */
/* EOF */
