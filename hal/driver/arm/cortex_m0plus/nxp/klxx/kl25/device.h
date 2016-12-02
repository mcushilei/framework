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


#ifndef __DRIVER_ARM_M0PLUS_NXP_KL25_DEVICE_H__
#define __DRIVER_ARM_M0PLUS_NXP_KL25_DEVICE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

typedef enum IRQn {
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn           = -14,    /*!< 2 Non Maskable Interrupt                           */
  HardFault_IRQn                = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                   */
  SVCall_IRQn                   = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                     */
  PendSV_IRQn                   = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                     */
  SysTick_IRQn                  = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                 */

/******  LPC12xx Specific Interrupt Numbers *******************************************************/
  WAKEUP0_IRQn                  = 0,        /*!< All I/O pins can be used as wakeup source.       */
  WAKEUP1_IRQn                  = 1,        /*!< There are 12 pins in total for LPC12xx           */
  WAKEUP2_IRQn                  = 2,
  WAKEUP3_IRQn                  = 3,
  WAKEUP4_IRQn                  = 4,   
  WAKEUP5_IRQn                  = 5,        
  WAKEUP6_IRQn                  = 6,        
  WAKEUP7_IRQn                  = 7,        
  WAKEUP8_IRQn                  = 8,        
  WAKEUP9_IRQn                  = 9,        
  WAKEUP10_IRQn                 = 10,       
  WAKEUP11_IRQn                 = 11,              
  I2C_IRQn                      = 12,       /*!< I2C Interrupt                                    */
  TIMER_16_0_IRQn               = 13,       /*!< 16-bit Timer0 Interrupt                          */
  TIMER_16_1_IRQn               = 14,       /*!< 16-bit Timer1 Interrupt                          */
  TIMER_32_0_IRQn               = 15,       /*!< 32-bit Timer0 Interrupt                          */
  TIMER_32_1_IRQn               = 16,       /*!< 32-bit Timer1 Interrupt                          */
  SSP_IRQn                      = 17,       /*!< SSP Interrupt                                    */
  UART0_IRQn                    = 18,       /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 19,       /*!< UART1 Interrupt                                  */
  CMP_IRQn                      = 20,       /*!< Comparator Interrupt                             */
  ADC_IRQn                      = 21,       /*!< A/D Converter Interrupt                          */
  WDT_IRQn                      = 22,       /*!< Watchdog timer Interrupt                         */  
  BOD_IRQn                      = 23,       /*!< Brown Out Detect(BOD) Interrupt                  */
  EINT0_IRQn                    = 25,       /*!< External Interrupt 0 Interrupt                   */
  EINT1_IRQn                    = 26,       /*!< External Interrupt 1 Interrupt                   */
  EINT2_IRQn                    = 27,       /*!< External Interrupt 2 Interrupt                   */
  DMA_IRQn                      = 29,       /*!< DMA Interrupt                                    */
  RTC_IRQn                      = 30,       /*!< RTC Interrupt                                    */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */

#include "..\..\..\common\common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
