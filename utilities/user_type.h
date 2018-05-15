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


#ifndef __APP_TYPE_H__
#define __APP_TYPE_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#ifndef FALSE
#   define FALSE            false
#endif

#ifndef TRUE
#   define TRUE             true
#endif

#ifndef ENABLED
#   define ENABLED          1
#endif

#ifndef DISABLED
#   define DISABLED         0
#endif

#define Hz                  *1u
#define KHz                 *1000u
#define MHz                 *1000000u
#define GHz                 *1000000000u

#define KB                  *1024u
#define MB                  *1024u*1024u
#define GB                  *1024u*1024u*1024u

/*============================ MACROFIED FUNCTIONS ===========================*/
#define __REG_MACRO__
//! \brief Macro to define bit field of register.
#define DEF_REG8                    \
    union {                         \
        struct {

#define END_DEF_REG8(__NAME)        \
        };                          \
        uint8_t Value;              \
    }__NAME;

#define TYPEDEF_REG8                \
    typedef union {                 \
        struct {

#define END_TYPEDEF_REG8(__NAME)    \
        };                          \
        uint8_t Value;              \
    }__NAME;

#define DEF_REG16                   \
    union {                         \
        struct {

#define END_DEF_REG16(__NAME)       \
        };                          \
        uint16_t Value;             \
    }__NAME;

#define TYPEDEF_REG16               \
    typedef union {                 \
        struct {

#define END_TYPEDEF_REG16(__NAME)   \
        };                          \
        uint16_t Value;             \
    }__NAME;

#define DEF_REG32                   \
    union {                         \
        struct {
    
#define END_DEF_REG32(__NAME)       \
        };                          \
        uint32_t Value;             \
    }__NAME;

#define TYPEDEF_REG32               \
    typedef union {                 \
        struct {

#define END_TYPEDEF_REG32(__NAME)   \
        };                          \
        uint32_t Value;             \
    }__NAME;

#define DEF_REGPAGE                 \
    typedef volatile struct {

#define END_DEF_REGPAGE(__NAME)     \
    } __NAME;

//! \brief Macro to reseverd register.
#define __8BIT_RSVD(__N, __A)       const uint8_t   :8;
#define REG8_RSVD(__N)              MREPEAT(__N, __8BIT_RSVD, 0)

#define __16BIT_RSVD(__N, __A)      const uint16_t  :16;
#define REG16_RSVD(__N)             MREPEAT(__N, __16BIT_RSVD, 0)

#define __32BIT_RSVD(__N, __A)      const uint32_t  :32;
#define REG32_RSVD(__N)             MREPEAT(__N, __32BIT_RSVD, 0)

/*============================ TYPES =========================================*/
#define __REG_TYPE__
typedef volatile uint8_t    reg8_t;
typedef volatile uint16_t   reg16_t;
typedef volatile uint32_t   reg32_t;

enum {
    FSM_RT_CPL = 0,     //!< fsm complete
    FSM_RT_ONGOING,
    FSM_RT_UNHANDLE,    //!< event unhandle
    FSM_RT_ERR,
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif // #ifndef __APP_TYPE_H__
