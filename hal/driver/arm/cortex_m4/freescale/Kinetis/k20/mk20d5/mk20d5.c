
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

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\device.h"
#include "..\..\..\..\common\core\core.h"
#include "..\..\common\wdt\wdt.h"
#include "..\..\common\mcg\mcg.h"

//#include "..\..\common\watchdog\watchdog.h"        //!< watchdog
//#include "..\..\common\uart\uart.h"                //!< uart driver
//#include "..\..\common\sleep\sleep.h"
//#include "..\..\common\flash\flash.h"

     
/*============================ MACROS ========================================*/
//! \name system clock definition
//! @{
#define CLK0_FREQ_HZ                        8000000
#define CLK0_TYPE                           CRYSTAL     
     
/*! \brief  PLL Configuration Info
 *! \note   The expected PLL output frequency is:
 *!         PLL out = (((CLKIN/PRDIV) x VDIV) / 2)
 *!         where the CLKIN can be either CLK0_FREQ_HZ or CLK1_FREQ_HZ.
 *! 
 *! \note For more info on PLL initialization refer to the mcg driver files.
 */
#define PLL0_PRDIV                          4
#define PLL0_VDIV                           24
     
//! @}
     

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/* for debug only */
volatile int32_t mcg_clk_hz;
volatile int32_t mcg_clk_khz;
volatile uint32_t core_clk_khz;
volatile int32_t periph_clk_khz;
volatile int32_t pll_clk_khz;
volatile int32_t core_clk_mhz;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**************************************************************
 **
 ** How to implement a low-level initialization function in C
 ** =========================================================
 **
 ** 1) Only use local auto variables.
 ** 2) Don't use global or static variables.
 ** 3) Don't use global or static objects (EC++ only).
 ** 4) Don't use agregate initializers, e.g. struct a b = {1};
 ** 5) Don't call any library functions (function calls that
 **    the compiler generates, e.g. to do integer math, are OK).
 **
 **************************************************************/

#if SYSTEM_LOW_LEVEL_INIT == ENABLED

WEAK bool BEFORE_SYSTEM_INIT(void)
{
    /* Return 1 to indicate that normal segment */
    /* initialization should be performed. If   */
    /* normal segment initialization should not */
    /* be performed, return 0.                  */

    return 1;
}

static void __fill_reg(reg32_t *pwREG, uint8_t chCount)
{
    do {
        *pwREG++ = 0xFFFFFFFF;
    } while(--chCount);
}

ROOT uint8_t __low_level_init(void)
{
    /* Uncomment the statement below if the RSTACK */
    /* segment has been placed in external SRAM!   */

    /* __require(__RSTACK_in_external_ram); */
    /* Or */
    /* __require(__RSTACK_in_external_ram_new_way); */
    /* Plus, if you uncomment this you will have to */
    /* define the sfr __?XMCRA in assembler         */

    /* If the low-level initialization routine is  */
    /* written in assembler, the line above should */
    /* be written as:                              */
    /*     EXTERN  __RSTACK_in_external_ram        */
    /*     REQUIRE __RSTACK_in_external_ram        */

    //! disable WDT and it's ahb clock    
    WDT.Disable();

    __fill_reg(ARM_NVIC.ICPR, 8);
    __fill_reg(ARM_NVIC.ISER, 8);

    SIM_SCGC_PORT = SIM_SCGC_PORTA_MSK | 
                    SIM_SCGC_PORTB_MSK | 
                    SIM_SCGC_PORTC_MSK | 
                    SIM_SCGC_PORTD_MSK | 
                    SIM_SCGC_PORTE_MSK;

    ARM_SCB.VTOR = 0x00000000;

    return BEFORE_SYSTEM_INIT();
}

#endif

//
//void vfnInitUSBClock (uint8_t u8ClkOption)
//{
//
//    switch (u8ClkOption)
//    {
//        case MCGPLL0:
//            SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK     /** PLL reference */   
//                    |  SIM_SOPT2_USBSRC_MASK;       /** USB fractional divider like USB reference clock */  
//            /** Divide reference clock by one to obtain 48MHz */
//            SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBDIV_MASK |SIM_CLKDIV2_USBFRAC_MASK);    
//            break;     
//
//        case MCGFLL:
//            /** Configure FLL to generate a 48MHz core clock */
//            //fll_init();                            
//              
//            SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;       /** FLL reference */   
//            SIM_SOPT2 |=  SIM_SOPT2_USBSRC_MASK;          /** USB fractional divider like USB reference clock */ 
//            /** Divide reference clock by one to obtain 48MHz */
//            SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBDIV_MASK |SIM_CLKDIV2_USBFRAC_MASK);      
//            break;
//
//        case CLKIN:
//            SIM_SOPT2 &= (uint32_t)(~SIM_SOPT2_USBSRC_MASK);    /** PTA5 selected as USBFS CLK source */ 
//            FLAG_SET(SIM_SCGC5_PORTA_SHIFT,SIM_SCGC5); 
//            PORTA_PCR25=(0|PORT_PCR_MUX(2));                   // Enabling PTA5 as CLK input  
//            break;
//    }
//
//    SIM_SCGC4|=(SIM_SCGC4_USBOTG_MASK);             // USB Clock Gating
//}

/*! \brief hardware initialization
 *! \param none
 *! \retval true run the default initialization
 *! \retval false ignore the default initialization
 */
bool clock_init(void)         
{
   
    /* Ramp up the system clock */
    /* Set the system dividers */
    /* NOTE: The PLL init will not configure the system clock dividers,
    * so they must be configured appropriately before calling the PLL
    * init function to ensure that clocks remain in valid ranges.
    */  
    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)
                    | SIM_CLKDIV1_OUTDIV2(0)
                    | SIM_CLKDIV1_OUTDIV4(1) );
    // releases hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
    // if ACKISO is set you must clear ackiso before calling pll_init 
    //    or pll init hangs waiting for OSC to initialize
    // if osc enabled in low power modes - enable it first before ack
    // if I/O needs to be maintained without glitches enable outputs and modules first before ack.
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK) {
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;
    }
    /* Initialize PLL */
    /* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
    mcg_clk_hz = pll_init(CLK0_FREQ_HZ,  /* CLKIN0 frequency */
                     LOW_POWER,     /* Set the oscillator for low power mode */
                     CLK0_TYPE,     /* Crystal or canned oscillator clock input */
                     PLL0_PRDIV,    /* PLL predivider value */
                     PLL0_VDIV,     /* PLL multiplier */
                     MCGOUT);       /* Use the output from this PLL as the MCGOUT */

    /* Check the value returned from pll_init() to make sure there wasn't an error */
    if (mcg_clk_hz < 0x100) {
        while(1);
    }
    
    /*
     * Use the value obtained from the pll_init function to define variables
	 * for the core clock in kHz and also the peripheral clock. These
	 * variables can be used by other functions that need awareness of the
	 * system frequency.
	 */
    mcg_clk_khz = mcg_clk_hz / 1000;
	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
  	periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
    
    return true;
}




//! \brief default hardware init routine
WEAK bool ON_HW_INIT(void)
{
    return true;
}

/*! \brief peripherals initialization
 *! \param none
 *! \return none
 */
bool driver_init(void)
{

    if (ON_HW_INIT()) {
        /* put default hardware initialization code here */
        clock_init();
    }
        
    return true;
}


/* EOF */