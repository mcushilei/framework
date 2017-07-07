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

//! \note do not move this pre-processor statement to other places
#define __CALENDAR_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define SECONDS_OF_MINUTE       (60u)
#define SECONDS_OF_HOUR         (60u * 60u)
#define SECONDS_OF_DAY          (24u * 60u * 60u)

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

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
bool is_leap_year(uint32_t year)
{
    if ((year % 400u == 0u) || ((year % 4u == 0u) && (year % 100u != 0u))) {
        return true;
    }

    return false;
}

static void validate_date(date_t *pDate)
{
    if (pDate->Year == 0) {
        pDate->Year = 1;
    }

    if (pDate->Month == 0) {
        pDate->Month = 1;
    } else if (pDate->Month > 12) {
        pDate->Month = 12;
    }

    if (pDate->Day == 0) {
        pDate->Day = 1;
    } else {
        uint8_t month = pDate->Month - 1u;

        if (is_leap_year(pDate->Year)) {
            static const uint8_t daysInMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (pDate->Day > daysInMonth[month]) {
                pDate->Day = daysInMonth[month];
            }
        } else {
            static const uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (pDate->Day > daysInMonth[month]) {
                pDate->Day = daysInMonth[month];
            }
        }
    }
}

uint32_t days_in_year(date_t *pDate)
{
    uint32_t i, days;
    uint8_t month;

    validate_date(pDate);

    days = 0;
    month = pDate->Month;
    month--;
    if (is_leap_year(pDate->Year)) {
        static const uint8_t daysInMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        for (i = 0; i < month; i++) {
            days += daysInMonth[i];
        }
    } else {
        static const uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        for (i = 0; i < month; i++) {
            days += daysInMonth[i];
        }
    }
    days += pDate->Day;

    return days;
}

//! count leap years from 1 to specified year, exclude specified year.
static uint32_t count_leap_years(uint32_t year)
{
    if (year == 0u) {
        return 0;
    }
    year--;
    return year / 4u - year / 100u + year / 400u;
}

//! count leap years from start to end year, exclude start and end year.
uint32_t count_leap_years_between(uint32_t year1, uint32_t year2)
{
    if (year1 == 0u) {
        year1 = 1u;
    }
    if (year2 == 0u) {
        year2 = 1u;
    }

    if (year1 == year2) {
        return 0;
    } else if (year1 > year2) {
        uint32_t t;
        t = year1;
        year1 = year2;
        year2 = t;
    }

    year1++;
    return count_leap_years(year2) - count_leap_years(year1);
}

uint32_t count_days_between(date_t *pDate1, date_t *pDate2)
{
    uint32_t days, years;
    date_t *pDate;

    do {
        if (pDate2->Year > pDate1->Year) {
            break;
        } else if (pDate2->Year == pDate1->Year) {
            if (pDate2->Month > pDate1->Month) {
                break;
            } else if (pDate2->Month == pDate1->Month) {
                if (pDate2->Day > pDate1->Day) {
                    return pDate2->Day - pDate1->Day;
                } else if (pDate2->Day == pDate1->Day) {
                    return 0;
                } else {
                    return pDate1->Day - pDate2->Day;
                }
            } else {
            }
        } else {
        }
        pDate  = pDate1;
        pDate1 = pDate2;
        pDate2 = pDate;
    } while (0);

    if (pDate1->Year != pDate2->Year) {
        if (is_leap_year(pDate1->Year)) {
            days = 366u - days_in_year(pDate1);
        } else {
            days = 365u - days_in_year(pDate1);
        }
        years = count_leap_years_between(pDate1->Year, pDate2->Year);
        days += 366u * years;
        days += 365u * (pDate2->Year - pDate1->Year - 1u - years);
        days += days_in_year(pDate2);
    } else {
        days = days_in_year(pDate2) - days_in_year(pDate1);
    }

    return days;
}

bool make_new_date_by_days(date_t *pDate, int32_t deltaDays)
{
    uint32_t year;
    uint8_t month;
    uint8_t day;
    date_t date;
    static const uint8_t daysInMonth[2][13] = {{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
    const uint8_t *pDaysInMonth = NULL;
    uint32_t days, diffDays;

    validate_date(pDate);
    year  = pDate->Year;
    month = pDate->Month;
    day   = pDate->Day;

    if (deltaDays < 0) {
        diffDays = -deltaDays;
__SUB_STATE_0:
        date.Year = year;
        date.Month = month;
        date.Day = day;
        days = days_in_year(&date);
        if (diffDays < days) {
        } else {
            diffDays -= days;
            year--;
            if (year == 0u) {
                return false;
            }
            month = 12;
            day = 31;

            while (diffDays > 365u) {
                if (is_leap_year(year)) {
                    if (diffDays > 366u) {
                        diffDays -= 366u;
                        year--;
                        if (year == 0u) {
                            return false;
                        }
                    } else {
                        break;
                    }
                } else {
                    diffDays -= 365u;
                    year--;
                    if (year == 0u) {
                        return false;
                    }
                }
            }
            goto __SUB_STATE_0;
        }

        if (is_leap_year(year)) {
            pDaysInMonth = (const uint8_t *)&daysInMonth[0];
        } else {
            pDaysInMonth = (const uint8_t *)&daysInMonth[1];
        }
        if (diffDays < day) {
            day -= diffDays;
        } else {
            diffDays -= day;
            month--;
            day = pDaysInMonth[month];
            while (diffDays >= day) {
                diffDays -= day;
                month--;
                day = pDaysInMonth[month];
            }
            day -= diffDays;
        }

    } else {
        diffDays = deltaDays;
__ADD_STATE_0:
        date.Year = year;
        date.Month = month;
        date.Day = day;
        if (is_leap_year(year)) {
            days = 366u - days_in_year(&date);
        } else {
            days = 365u - days_in_year(&date);
        }
        if (diffDays <= days) {
        } else {
            diffDays -= days + 1u;
            year++;
            month = 1;
            day = 1;

            while (diffDays > 365u) {
                if (is_leap_year(year)) {
                    if (diffDays > 366u) {
                        diffDays -= 366u;
                        year++;
                    } else {
                        break;
                    }
                } else {
                    diffDays -= 365u;
                    year++;
                }
            }
            goto __ADD_STATE_0;
        }

        if (is_leap_year(year)) {
            pDaysInMonth = (const uint8_t *)&daysInMonth[0];
        } else {
            pDaysInMonth = (const uint8_t *)&daysInMonth[1];
        }
        if (diffDays <= (pDaysInMonth[month] - day)) {
            day += diffDays;
        } else {
            diffDays -= pDaysInMonth[month] - day + 1u;
            day = 1u;
            month++;
            while (diffDays > 28u) {
                if (diffDays > pDaysInMonth[month]) {
                    diffDays -= pDaysInMonth[month];
                    month++;
                } else {
                    break;
                }
            }
            day += diffDays;
            while (day > pDaysInMonth[month]) {
                day -= pDaysInMonth[month];
                month++;
            }
        }
    }

    pDate->Year  = year;
    pDate->Month = month;
    pDate->Day   = day;

    return true;
}

static void validate_time(rtime_t *pTime)
{
    pTime->Hour %= 24u;
    pTime->Minute %= 60u;
    pTime->Second %= 60u;
}

uint32_t time_to_seconds(rtime_t *pTime)
{
    validate_time(pTime);
    return 60u * 60u * pTime->Hour + 60u * pTime->Minute + pTime->Second;
}

void seconds_to_time(rtime_t *pTime, uint32_t seconds)
{
    uint32_t days;

    days = seconds / SECONDS_OF_DAY;
    seconds -= days * SECONDS_OF_DAY;
    pTime->Hour = seconds / SECONDS_OF_HOUR;
    seconds -= pTime->Hour * SECONDS_OF_HOUR;
    pTime->Minute = seconds / SECONDS_OF_MINUTE;
    seconds -= pTime->Minute * SECONDS_OF_MINUTE;
    pTime->Second = seconds;
}

uint32_t seconds_between_time(rtime_t *pTime1, rtime_t *pTime2)
{
    uint32_t s1, s2;

    s1 = time_to_seconds(pTime1);
    s2 = time_to_seconds(pTime2);
    if (s1 < s2) {
        return s2 - s1;
    } else {
        return s1 - s2;
    }
}

/* EOF */
