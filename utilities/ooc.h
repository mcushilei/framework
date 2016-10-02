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

#ifndef __OBJECT_ORIENTED_C_H__
#define __OBJECT_ORIENTED_C_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
//! \name class definition
//! @{
#define CLASS(__NAME)                   __##__NAME

#define DEF_CLASS(__NAME, ...)          \
            typedef union __NAME __NAME;\
            typedef struct __##__NAME __##__NAME;\
            __VA_ARGS__                 \
            struct __##__NAME {

#define END_DEF_CLASS(__NAME)           \
            };                          \
            union __NAME {              \
                struct __##__NAME;      \
                uint_fast8_t __MSK[(sizeof(__##__NAME) + sizeof(uint_fast8_t) - 1) / sizeof(uint_fast8_t)];\
            };

#define EXTERN_CLASS(__NAME, ...)       \
            typedef union __NAME __NAME;\
            __VA_ARGS__                 \
            union __NAME {              \
                uint_fast8_t __MSK[(sizeof(struct {

#define END_EXTERN_CLASS(__NAME)        \
                }) + sizeof(uint_fast8_t) - 1) / sizeof(uint_fast8_t)];\
            };
//! @}

//! \name interface definition
//! @{
#define DEF_INTERFACE(__NAME, ...)      \
            typedef struct __NAME __NAME;\
            __VA_ARGS__                 \
            struct __NAME {

#define END_DEF_INTERFACE(__NAME)       \
            };
//! @}

//! \brief macro for inheritance
#define INHERIT_EX(__TYPE, __NAME)      \
            union {                     \
                __TYPE;                 \
                __TYPE  __NAME;         \
            };

#define IMPLEMENT(__INTERFACE)          INHERIT_EX(__INTERFACE, __##__INTERFACE)
#define INHERIT(__TYPE)                 INHERIT_EX(__TYPE, base)

/*============================ TYPES =========================================*/
//! \name interface: u32_property_t
//! @{
DEF_INTERFACE(u32_property_t)
    bool            (*Set)(uint_fast32_t wValue);
    uint_fast32_t   (*Get)(void);
END_DEF_INTERFACE(u32_property_t)
//! @}

//! \name interface: u16_property_t
//! @{
DEF_INTERFACE(u16_property_t)
    bool            (*Set)(uint_fast16_t wValue);
    uint_fast16_t   (*Get)(void);
END_DEF_INTERFACE(u16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
DEF_INTERFACE(u8_property_t)
    bool            (*Set)(uint_fast8_t wValue);
    uint_fast8_t    (*Get)(void);
END_DEF_INTERFACE(u8_property_t)
//! @}


//! \name interface: i32_property_t
//! @{
DEF_INTERFACE(i32_property_t)
    bool            (*Set)(int_fast32_t wValue);
    int_fast32_t    (*Get)(void);
END_DEF_INTERFACE(i32_property_t)
//! @}

//! \name interface: i16_property_t
//! @{
DEF_INTERFACE(i16_property_t)
    bool            (*Set)(int_fast16_t wValue);
    int_fast16_t    (*Get)(void);
END_DEF_INTERFACE(i16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
DEF_INTERFACE(i8_property_t)
    bool            (*Set)(int_fast8_t wValue);
    int_fast8_t     (*Get)(void);
END_DEF_INTERFACE(i8_property_t)
//! @}


//! \name interface: bool_property_t
//! @{
DEF_INTERFACE(bool_property_t)
    bool (*Set)(bool bValue);
    bool (*Get)(void);
END_DEF_INTERFACE(bool_property_t)
//! @}

//! \name interface: bool_property_t
//! @{
DEF_INTERFACE(en_property_t)
    bool (*Enable)(void);
    bool (*Disable)(void);
END_DEF_INTERFACE(en_property_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
