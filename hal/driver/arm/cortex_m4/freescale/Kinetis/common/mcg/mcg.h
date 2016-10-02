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

#ifndef __DRIVER_COMMON_MCG_H__
#define __DRIVER_COMMON_MCG_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/

// Constants for use in pll_init
#define NO_OSCINIT              0
#define OSCINIT                 1

#define OSC_0                   0
#define OSC_1                   1

#define LOW_POWER               0
#define HIGH_GAIN               1

#define CANNED_OSC              0
#define CRYSTAL                 1

#define PLL_0                   0
#define PLL_1                   1

#define PLL_ONLY                0
#define MCGOUT                  1

// MCG Mode defines
/*
#define FEI  1
#define FEE  2
#define FBI  3
#define FBE  4
#define BLPI 5
#define BLPE 6
#define PBE  7
#define PEE  8
*/

//! \name clock modes
//! @{
#define BLPI 1
#define FBI  2
#define FEI  3
#define FEE  4
#define FBE  5
#define BLPE 6
#define PBE  7
#define PEE  8
//! @}

// IRC defines
#define SLOW_IRC                0
#define FAST_IRC                1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*! \name : pll_init
 *! \brief  Mode transition: Option to move from FEI to PEE mode or to just 
 *!         initialize the PLL
 *!         This function initializess either PLL0 or PLL1. Either OSC0 or OSC1  
 *!         selected for the reference clock source. The oscillators can be 
 *!         configured to use a crystal or take in an external square wave clock.
 *!
 *! \note   This driver does not presently (as of Sept 9 2011) support the use 
 *!         of OSC1 as the reference clock for the MCGOUT clock used for the 
 *!         system clocks.
 *!
 *! The PLL outputs a PLLCLK and PLLCLK2X. PLLCLK2X is the actual PLL frequency 
 *! and PLLCLK is half this frequency. PLLCLK is used for MCGOUT and is also 
 *! typically used by the peripherals that can select the PLL as a clock source. 
 *! So the PLL frequency generated will be twice the desired frequency.
 *! Using the function parameter names the PLL frequency is calculated as follows:
 *! PLL freq = ((crystal_val / prdiv_val) * vdiv_val)
 *! Refer to the readme file in the mcg driver directory for examples of pll_init 
 *! configurations. All parameters must be provided, for example crystal_val must 
 *! be provided even if the oscillator associated with that parameter is already 
 *! initialized. The various passed parameters are checked to ensure they are 
 *! within the allowed range. If any of these checks fail the driver will exit 
 *! and return a fail/error code. An error code will also be returned if any 
 *! error occurs during the PLL initialization sequence. Refer to the readme file 
 *! in the mcg driver directory for a list of all these codes.
 *!
 *! \param crystal_val - external clock frequency in Hz either from a crystal 
 *                       or square wave clock source
 *! \param hgo_val     - selects whether low power or high gain mode is selected
 *                       for the crystal oscillator. This has no meaning if an
 *                       external clock is used.
 *! \param erefs_val   - selects external clock (=0) or crystal osc (=1)
 *! \param prdiv_val   - value to divide the external clock source by to create 
 *                       the desired PLL reference clock frequency
 *! \param vdiv_val    - value to multiply the PLL reference clock frequency by
 *! \param mcgout_select  - 0 if the PLL is just to be enabled, non-zero if the 
 *                       PLL is used to provide the MCGOUT clock for the system.
 *! \return PLL frequency (Hz) divided by 2 or error code
 */
extern int32_t pll_init(int32_t crystal_val, uint8_t hgo_val, uint8_t erefs_val, 
                 int8_t prdiv_val, int8_t vdiv_val, uint8_t mcgout_select);

#endif /* __MCG_H__ */
