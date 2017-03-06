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

#ifndef __ULTILITIES_H__
#define __ULTILITIES_H__

/*============================ INCLUDES ======================================*/
#include ".\compiler.h"
#include ".\user_type.h"
#include ".\preprocessor\preprocessor.h"
#include ".\usebits.h"
#include ".\ooc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
//! \name structure definition
//! @{
#define DEF_STRUCTURE(__NAME,...)           \
            typedef struct __NAME __NAME;   \
            __VA_ARGS__                     \
            struct __NAME {

#define END_DEF_STRUCTURE(__NAME)           \
            };
//! @}

#define ARRAY_LENGTH(__ARRAY)     (sizeof(__ARRAY) / sizeof(__ARRAY[0]))

/*! \brief  change representation of a varible to another type. This is different
 *          from type conversion which change the BINARY value of varible.
 *  \note   It's assume that the destination type is memory alligned to original
 *          type.
 */
#define TYPE_CAST(__V, __T) ((__T *)&(__V))[0]

#define IS_POWER_OF_2(__N)  (((__N) != 0u) && ((((__N) - 1u) & (__N)) == 0))

#define NEXT_POEWER_OF_2(__N, __V)          do {\
            uint32_t x = __N;                   \
            if (IS_POWER_OF_2(x)) {             \
                __V = x;                        \
                break;                          \
            }                                   \
            x |= x >> 1;                        \
            x |= x >> 2;                        \
            x |= x >> 4;                        \
            x |= x >> 8;                        \
            x |= x >> 16;                       \
            __V = x + 1;                        \
        } while (0)

#define COUNT_TRAILING_ZEROS(__N, __V)      do {\
            uint32_t x = __N;                   \
            x = ((x - 1) | x) ^ x;              \
            x = (x & 0x55555555U) + ((x >> 1)  & 0x55555555U);\
            x = (x & 0x33333333U) + ((x >> 2)  & 0x33333333U);\
            x = (x & 0x0f0f0f0fU) + ((x >> 4)  & 0x0f0f0f0fU);\
            x = (x & 0x00ff00ffU) + ((x >> 8)  & 0x00ff00ffU);\
            x = (x & 0x0000ffffU) + ((x >> 16) & 0x0000ffffU);\
            __V = x;                            \
        } while (0)

#define COUNT_LEADING_ZEROS(__N, __V)       do {\
            uint32_t x = __N;                   \
            x |= x >> 1;                        \
            x |= x >> 2;                        \
            x |= x >> 4;                        \
            x |= x >> 8;                        \
            x |= x >> 16;                       \
            x = ~x;                             \
            x = (x & 0x55555555U) + ((x >> 1)  & 0x55555555U);\
            x = (x & 0x33333333U) + ((x >> 2)  & 0x33333333U);\
            x = (x & 0x0f0f0f0fU) + ((x >> 4)  & 0x0f0f0f0fU);\
            x = (x & 0x00ff00ffU) + ((x >> 8)  & 0x00ff00ffU);\
            x = (x & 0x0000ffffU) + ((x >> 16) & 0x0000ffffU);\
            __V = x;                            \
        } while (0)

//! \brief math macros
#define MAX(__A,__B)        (((__A) > (__B)) ? (__A) : (__B))
#define MIN(__A,__B)        (((__A) < (__B)) ? (__A) : (__B))
#define ABS(__I)            (((__I) ^ ((__I) >> 31)) - ((__I) >> 31))
            
//! \brief LFSM is designed to be used in thread (in OS) or in singal task (in RAW system);
//! A local FSM is a function in C.
#define LFSM_DEFINE(__M)    void __M(void *pParam)
#define LFSM_PROTOTYPE(__M) void __M(void *pParam);
#define LFSM_RUN(__M, __P)  __M(__P);
    
#define LFSM_BEGIN(__M)     do {\
    
#define LFSM_END(__M)       __##__M##:\
                            break;\
                            } while (0);

#define LFSM_CPL(__M)       goto __##__M##;

//! A state is one code block in C.
#define LFSM_STATE_BEGIN(__S)       __##__S##:\
                                    {

//! It reinto this state if there is no state transfer.
#define LFSM_STATE_END(__S)         }\
                                    goto __##__S##;

#define LFSM_STATE_TRANS_TO(__S)    goto __##__S##;

/* LFSM example:
LFSM_DEFINE(test_fsm)
{
    LFSM_BEGIN(test_fsm)
        uint8_t c = 4;
        LFSM_STATE_BEGIN(state_1)
            printf("\r\nc = %u", c);
            LFSM_STATE_TRANS_TO(state_2)
        LFSM_STATE_END(state_1)

        LFSM_STATE_BEGIN(state_2)
            uint8_t d = 4;
            printf("\r\nd = %u", d);
            c--;
            if (c == 0) {
                LFSM_CPL(test_fsm)
            } else {
                LFSM_STATE_TRANS_TO(state_1)
            }
        LFSM_STATE_END(state_2)
    LFSM_END(test_fsm)
}
*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __TEMPLATE_H__
/* EOF */
