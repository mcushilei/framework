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

#ifndef __PM_H__
#define __PM_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\reg_syscon.h"
#include ".\reg_pmu.h"

/*============================ MACROS ========================================*/
#define DIV_(_N,_D)     DIV_##_N = (_N),

#define ON_SLEEP_TURN_ON(__MSK)             (__MSK)
#define ON_SLEEP_TURN_OFF(__MSK)            (~(__MSK))
#define ON_WAKE_UP_TURN_ON(__MSK)           (__MSK)
#define ON_WAKE_UP_TURN_OFF(__MSK)          (~(__MSK))
#define ON_SEMI_WAKE_UP_TURN_ON(__MSK)      (__MSK)
#define ON_SEMI_WAKE_UP_TURN_OFF(__MSK)     (~(__MSK))
#define ON_ZOMBIE_TURN_ON(__MSK)            (__MSK)
#define ON_ZOMBIE_TURN_OFF(__MSK)           (~(__MSK))
#define ON_SLEEP_WALK_TURN_ON(__MSK)        (__MSK)
#define ON_SLEEP_WALK_TURN_OFF(__MSK)       (~(__MSK))



/*============================ MACROFIED FUNCTIONS ===========================*/
#define PM_MAIN_CLK_GET()                   main_clk_get()
#define PM_CORE_CLK_GET()                   core_clk_get()
#define PM_AHB_CLK_GET_STATUS(__INDEX)      ahb_clock_get_status((__INDEX))
#define PM_AHB_CLK_ENABLE(__INDEX)          ahb_clock_enable((__INDEX))
#define PM_AHB_CLK_DISABLE(__INDEX)         ahb_clock_disable((__INDEX))
#define PM_AHB_CLK_RESUME(__INDEX, __STATUS) ahb_clock_resume_status((__INDEX), (__STATUS))
#define PM_PCLK_CFG(__INDEX, __DIV)         peripheral_clk_config((__INDEX), (__DIV))
#define PM_PCLK_GET(__INDEX)                peripheral_clk_get((__INDEX))
#define PM_PCLK_GET_STATUS(__INDEX)         peripheral_clk_get_div((__INDEX))
#define PM_PCLK_RESUME(__INDEX, __STATUS)   peripheral_clk_config((__INDEX), (__STATUS))

/*============================ TYPES =========================================*/
//! \name the lowpower mode
//! @{
typedef enum {
    WAIT            = 0,
    SLEEP           = 1,
    DEEP_SLEEP      = 2,
    POWER_DOWN      = 3,
} em_lowpower_mode_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool     power_enable(uint32_t Part);
extern bool     power_disable(uint32_t Part);
extern uint32_t power_status_get(uint32_t Part);
extern bool     power_status_resume(uint32_t Part , uint32_t Status);

extern bool     pll_enable(void);
extern bool     pll_disable(void);
//! Before call: PLL should NOT be in use AND should be power down.
//! After  call: If there is any error(return false), PLL is power down;
//!              else, PLL is work well.
extern bool     pll_config(uint32_t Source, uint32_t Freq);
extern uint32_t pll_get_in_clock(void);
extern uint32_t pll_get_out_clock(void);

extern bool     ahb_clock_enable(uint32_t Part);
extern bool     ahb_clock_disable(uint32_t Part);
extern uint32_t ahb_clock_get_status(uint32_t Part);
extern bool     ahb_clock_resume_status(uint32_t Part, uint32_t Status);
extern bool     ahb_clock_config(uint32_t Source, uint32_t Div);
extern uint32_t ahb_clock_get(void);
extern uint32_t main_clock_get(void);

extern bool     peripheral_clk_config(uint8_t chIndex , uint8_t chDiv);
extern uint32_t peripheral_clk_get_div(uint8_t chIndex);
extern uint32_t peripheral_clk_get(uint8_t chIndex);

extern bool     scon_usart_clock_enable(void);
extern bool     scon_usart_clock_disable(void);
extern uint32_t scon_usart_clock_get(void);

extern bool     clock_out_config(uint8_t tSrc, uint32_t wDiv);
extern bool     enter_lowpower_mode(em_lowpower_mode_t tSleep);

#endif
