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
} rtc_t;
//! @}

/*============================ PROTOTYPES ====================================*/
static bool rtc_init(void);
static void rtc_enable(void);
static void rtc_disable(void);
static uint32_t rtc_get_time_value(void);
static bool     rtc_set_time_value(uint32_t wValue);
static uint32_t rtc_get_match_value(void);
static bool     rtc_set_match_value(uint32_t wValue);


/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the RTC
const rtc_t RTC = {    
    &rtc_init,                                  //!< initialize the RTC
    &rtc_enable,                                //!< enable the ahbclk
    &rtc_disable,                               //!< disable the ahbclk
    {
        &rtc_set_time_value,                    //!< set count value
        &rtc_get_time_value,                    //!< get count value
    },
    {
        &rtc_set_match_value,                   //!< set match value
        &rtc_get_match_value,                  //!< get match value
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*!\brief init real-time clock
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
static bool rtc_init(void)
{
    __SAFE_CLK_CODE (
        RTC_REG.CCR     = (1u << 0)         //!< clock is enabled.
                        | (0u << 1)         //!< not reset.
                        | (0u << 4);        //!< calibration is not enabled.
        RTC_REG.CIIR    = (1u << 0)         //!< increment of second will trige an interrupt.
                        | (0u << 1)         //!< increment of minute will not trige an interrupt.
                        | (0u << 1)         //!< increment of hour will not trige an interrupt.
                        | (0u << 1)         //!< increment of day of month will trige an interrupt.
                        | (0u << 1)         //!< increment of day of week will not trige an interrupt.
                        | (0u << 1)         //!< increment of day of yeat will not trige an interrupt.
                        | (0u << 1)         //!< increment of month will not trige an interrupt.
                        | (0u << 1);        //!< increment of year will not trige an interrupt.
    )
    return true;

}

/*!\brief enable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_enable(void)
{
    peripheral_clock_enable(PCONP_RTC);
}

/*!\brief disable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_disable(void)
{
    peripheral_clock_disable(PCONP_RTC);
}

/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
static uint32_t rtc_get_time_value(void)
{
    return 0;
}
/*!\brief set count value
 *! \param void
 *! \retval return geh time count value
 */
static bool rtc_set_time_value(uint32_t wValue)
{
    return true;
}

/*!\brief Set match value
 *! \param time match value
 *! \retval none
 */
static bool rtc_set_match_value(uint32_t wValue)
{
    return true;
}

/*!\brief get match value
 *! \param void
 *! \retval return match value
 */
static uint32_t rtc_get_match_value(void)
{
    return 0;
}