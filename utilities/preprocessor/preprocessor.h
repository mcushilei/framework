/***************************************************************************
 *   Copyright(C)2009-2014 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
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

#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

/*============================ INCLUDES ======================================*/
#include "tpaste.h"
#include "stringz.h"
#include "mrepeat.h"

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

#define UBOUND(__ARRAY)     (sizeof(__ARRAY) / sizeof(__ARRAY[0]))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif  //! #ifndef __PREPROCESSOR_H__
