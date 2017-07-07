/*******************************************************************************
 *  Copyright(C)2016 by Dreistein<mcu_shilei@hotmail.com>                     *
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

#ifndef __RTC_H__
#define __RTC_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\reg_rtc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name rtc struct
//! @{
typedef struct {    
    bool                (*Init)(void);   //!< initialize the RTC
    void                (*Enable)(void);            //!< enable the ahbclk
    void                (*Disable)(void);           //!< disable the ahbclk
    u32_property_t      Count;                      //!<  count value
    u32_property_t      Match;                 //!<  match value
} rtc_t;
//! @}

typedef struct {
    uint32_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
} rtc_time_t;

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the PMU
extern const rtc_t RTC;

/*============================ PROTOTYPES ====================================*/
extern bool rtc_init(void);
extern void rtc_enable(void);
extern void rtc_disable(void);
extern void rtc_get_time_value(rtc_time_t* pTime);
extern void rtc_set_time_value(rtc_time_t* pTime);
extern void rtc_set_time_match_value(rtc_time_t* pTime);
extern void rtc_get_time_match_value(rtc_time_t* pTime);

#endif
