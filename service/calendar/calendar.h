/*******************************************************************************
 *  Copyright(C)2017 by Dreistein<mcu_shilei@hotmail.com>                     *
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


#ifndef __CALENDAR_C__
#ifndef __CALENDAR_H__
#define __CALENDAR_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct {
    uint32_t    Year;
    uint8_t     Month;
    uint8_t     Day;
} date_t;

typedef struct {
    uint8_t     Hour;
    uint8_t     Minute;
    uint8_t     Second;
} rtime_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern bool     is_leap_year(uint32_t year);
extern uint32_t days_of_year(date_t *pDate);
extern uint32_t count_leap_years_between(uint32_t year1, uint32_t year2);
extern uint32_t count_days_between(date_t *pDate1, date_t *pDate2);
extern bool     make_new_date_by_days(date_t *pDate, int32_t deltaDays);

extern uint32_t seconds_of_day(rtime_t *pTime);
extern uint32_t seconds_between_time(rtime_t *pTime1, rtime_t *pTime2);

#endif  //! #ifndef __CALENDAR_H__
#endif  //! #ifndef __CALENDAR_C__
/* EOF */
