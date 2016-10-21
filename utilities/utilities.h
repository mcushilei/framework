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
            
//! \brief initialize large object
# define OBJECT_INIT_ZERO(__OBJECT)         do {\
            struct OBJECT_INIT {\
                uint8_t StructMask[sizeof(__OBJECT)];\
            } NULL_OBJECT = {{0}};\
            (*((struct OBJECT_INIT *)&(__OBJECT))) = NULL_OBJECT;\
        } while (false)

//! \brief math macros
#define MAX(__A,__B)        (((__A) > (__B)) ? (__A) : (__B))
#define MIN(__A,__B)        (((__A) < (__B)) ? (__A) : (__B))
#define ABS(__N)            (((__N) < 0)? (-(__N)) : (__N))
#define IS_POWER_OF_2(__N)  (   ((__N) == 0)                                \
                             || (((__N) != 1u) && !(((__N) - 1u) & (__N))))

#define LFSM_BEGIN(__M)     do {
    
#define LFSM_END(__M)       __##__M##:\
                            break;\
                            } while (0);

#define LFSM_CPL(__M)       goto __##__M##;

#define LFSM_STATE_BEGIN(__S)       __##__S##:\
                                    {
    
#define LFSM_STATE_END(__S)         }\
                                    goto __##__S##;

#define LFSM_STATE_TRANS_TO(__S)    goto __##__S##;
                                    
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __TEMPLATE_H__
/* EOF */
