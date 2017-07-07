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

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\reg_rtc.h"
#include "..\scon\pm.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __SAFE_CLK_CODE(...)                     {                      \
        uint32_t tAHBStatus = peripheral_clock_get_status(PCONP_RTC);   \
        peripheral_clock_enable(PCONP_RTC);                             \
        __VA_ARGS__;                                                    \
        peripheral_clock_resume_status(PCONP_RTC,tAHBStatus);           \
    }

/*============================ TYPES =========================================*/
//! \name rtc struct
//! @{
typedef struct {    
    bool                (*Init)(void);          //!< initialize the RTC
    void                (*Enable)(void);        //!< enable the ahbclk
    void                (*Disable)(void);       //!< disable the ahbclk
    u32_property_t      Count;                  //!< count value
    u32_property_t      Match;                  //!< match value
} i_rtc_t;
//! @}

typedef struct {
    uint32_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
} rtc_time_t;

/*============================ PROTOTYPES ====================================*/
bool rtc_init(void);
void rtc_enable(void);
void rtc_disable(void);
uint32_t rtc_get_counter_value(void);
bool     rtc_set_counter_value(uint32_t wValue);
uint32_t rtc_get_counter_match_value(void);
bool     rtc_set_counter_match_value(uint32_t wValue);


/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the RTC
const i_rtc_t RTC = {    
    &rtc_init,                                  //!< initialize the RTC
    &rtc_enable,                                //!< enable the ahbclk
    &rtc_disable,                               //!< disable the ahbclk
    {
        &rtc_set_counter_value,                    //!< set count value
        &rtc_get_counter_value,                    //!< get count value
    },
    {
        &rtc_set_counter_match_value,       //!< set match value
        &rtc_get_counter_match_value,       //!< get match value
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*!\brief init real-time clock
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
bool rtc_init(void)
{
    __SAFE_CLK_CODE (
        RTC_REG.CCR     = (1u << 0)         //!< clock is enabled.
                        | (0u << 1)         //!< not reset.
                        | (0u << 4);        //!< calibration is not enabled.
        RTC_REG.CIIR    = (0u << 0)         //!< increment of second will not trige an interrupt.
                        | (0u << 1)         //!< increment of minute will not trige an interrupt.
                        | (0u << 1)         //!< increment of hour   will not trige an interrupt.
                        | (0u << 1)         //!< increment of day of month will trige an interrupt.
                        | (0u << 1)         //!< increment of day of week  will not trige an interrupt.
                        | (0u << 1)         //!< increment of day of yeat  will not trige an interrupt.
                        | (0u << 1)         //!< increment of month  will not trige an interrupt.
                        | (0u << 1);        //!< increment of year   will not trige an interrupt.
    )
    return true;
}

/*!\brief enable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
void rtc_enable(void)
{
    peripheral_clock_enable(PCONP_RTC);
}

/*!\brief disable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
void rtc_disable(void)
{
    peripheral_clock_disable(PCONP_RTC);
}

/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
uint32_t rtc_get_counter_value(void)
{
    return 0;
}
/*!\brief set count value
 *! \param void
 *! \retval return geh time count value
 */
bool rtc_set_counter_value(uint32_t wValue)
{
    return true;
}

/*!\brief Set match value
 *! \param time match value
 *! \retval none
 */
bool rtc_set_counter_match_value(uint32_t wValue)
{
    return true;
}

/*!\brief get match value
 *! \param void
 *! \retval return match value
 */
uint32_t rtc_get_counter_match_value(void)
{
    return 0;
}



void rtc_get_time_value(rtc_time_t* pTime)
{
    __SAFE_CLK_CODE (
        pTime->Year  = RTC_REG.YEAR;
        pTime->Month = RTC_REG.MONTH;
        pTime->Day   = RTC_REG.DOM;
        pTime->Hour  = RTC_REG.HOUR;
        pTime->Minute = RTC_REG.MIN;
        pTime->Second = RTC_REG.SEC;
    )
}

void rtc_set_time_value(rtc_time_t* pTime)
{
    __SAFE_CLK_CODE (
        RTC_REG.SEC = pTime->Second;
        RTC_REG.MIN = pTime->Minute;
        RTC_REG.HOUR  = pTime->Hour;
        RTC_REG.DOM   = pTime->Day;
        RTC_REG.MONTH = pTime->Month;
        RTC_REG.YEAR  = pTime->Year;
    )
}

void rtc_set_time_match_value(rtc_time_t* pTime)
{
    __SAFE_CLK_CODE (
        RTC_REG.ALSEC = pTime->Second;
        RTC_REG.ALMIN = pTime->Minute;
        RTC_REG.ALHOUR  = pTime->Hour;
        RTC_REG.ALDOM   = pTime->Day;
        RTC_REG.ALMON   = pTime->Month;
        RTC_REG.ALYEAR  = pTime->Year;
    )
}

void rtc_get_time_match_value(rtc_time_t* pTime)
{
    __SAFE_CLK_CODE (
        pTime->Year  = RTC_REG.ALYEAR;
        pTime->Month = RTC_REG.ALMON;
        pTime->Day   = RTC_REG.ALDOM;
        pTime->Hour  = RTC_REG.ALHOUR;
        pTime->Minute = RTC_REG.ALMIN;
        pTime->Second = RTC_REG.ALSEC;
    )
}

