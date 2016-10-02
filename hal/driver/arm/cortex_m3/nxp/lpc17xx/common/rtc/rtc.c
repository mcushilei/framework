/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\i_io_rtc.h"
#include "..\pm\pm.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define SAFE_CLK_CODE(...)                     {                            \
        ahbclk_status_t tAHBStatus = PM_GET_AHB_CLK_STATUS(AHBCLK_RTC0);    \
        PM_AHB_CLK_ENABLE(AHBCLK_RTC0);                                     \
        __VA_ARGS__;                                                        \
        PM_AHB_CLK_RESUME(AHBCLK_RTC0,tAHBStatus);                          \
    }

/*============================ TYPES =========================================*/
//! \name RTC initialization arguments defination
//! @{
typedef enum {
    //! Enable the RTC
    DISABLE_RTC             = 0x00,
    //! Enable the RTC
    ENABLE_RTC              = 0x01, 
    //! Select AON_OSC as clock source
    RTC_SRC_AON             = 0x00,
    //! Select 1HZ 32KOSC output
    RTC_SRC_32KOSC_1HZ      = 0x02, 
    //! Select 1KHz 32KOSC output
    RTC_SRC_32KOSC_1KHZ     = 0x0E,
    //! Enable the RTC interrupt
    RTC_INTERRUPT_DISABLE   = 0x00,
    //! Enable the RTC interrupt
    RTC_INTERRUPT_ENABLE    = 0x10,
} em_rtc_cfg_mode_t;
//! @}

//! \name RTC config struct
//! @{
typedef struct {
    uint8_t         chMode;                      //!< Watchdog config ward
    uint32_t        wMatchValue;
}rtc_cfg_t;
//! @}

//! \name rtc property type
//! @{
DEF_INTERFACE(rtc_property_int_t)
    bool        (*Get)(void);
    void        (*Clear)(void);  
END_DEF_INTERFACE(rtc_property_int_t)
//! @}

//! \name rtc struct
//! @{
typedef struct {    
    bool                (*Init)(rtc_cfg_t *tCfg);   //!< initialize the RTC
    void                (*Enable)(void);            //!< enable the ahbclk
    void                (*Disable)(void);           //!< disable the ahbclk
    u32_property_t      Count;        //!<  count value
    u32_property_t      Match;                 //!<  match value
    rtc_property_int_t  Flag;                       //!<  interrupt flag
    rtc_reg_t   *const  RegPage;                     //!< register page
} rtc_t;
//! @}
/*============================ PROTOTYPES ====================================*/
/*!\brief init real-time clock
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
static bool init_realtime_clock(rtc_cfg_t *tCfg);
/*!\brief enable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_enable(void);
/*!\brief disable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_disable(void);
/*!\brief Clear the rtc flag
 *! \param none
 *! \retval none
 */
static void rtc_clear_flag(void);

/*!\brief get the rtc flag
 *! \param none
 *! \retval none
 */
static bool rtc_get_flag(void);

/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
static uint32_t rtc_get_time_value(void);
/*!\brief set count value
 *! \param void
 *! \retval return geh time count value
 */
static bool rtc_set_time_value(uint32_t wValue);
/*!\brief get match value
 *! \param void
 *! \retval return match value
 */
static uint32_t rtc_read_match_value(void);

/*!\brief Set match value
 *! \param time match value
 *! \retval none
 */
static bool rtc_set_match_value(uint32_t wValue);


/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the RTC
const rtc_t RTC = {    
    &init_realtime_clock,                       //!< initialize the RTC
    &rtc_enable,                                //!< enable the ahbclk
    &rtc_disable,                               //!< disable the ahbclk
    {
        &rtc_set_time_value,                    //!< set count value
        &rtc_get_time_value,                    //!< get count value
    },
    {
        &rtc_set_match_value,                   //!< set match value
        &rtc_read_match_value,                  //!< get match value
    },
    {
        &rtc_get_flag,
        &rtc_clear_flag,                        //!< clear interrupt flag
    },

    ((rtc_reg_t *)(RTC_BASE_ADDRESS)),         //!< register page
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*!\brief init real-time clock
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
static bool init_realtime_clock(rtc_cfg_t *tCfg)
{
    if ( NULL == tCfg ) {
        return false;
    }
    SAFE_CLK_CODE (
        GSP_RTC.WKUPCLR = 0x01;
        GSP_RTC.CR.Value= (tCfg->chMode & 0x0F );
        GSP_RTC.IMSC.Value = (tCfg->chMode >> 4);
        GSP_RTC.WKUPEN = (tCfg->chMode >> 4);
        GSP_RTC.MATCH = tCfg->wMatchValue;
        GSP_RTC.LOAD = 0;
    )
    return true;

}

/*!\brief enable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_enable(void)
{
    PM_AHB_CLK_ENABLE(AHBCLK_RTC0);
}

/*!\brief disable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static void rtc_disable(void)
{
    PM_AHB_CLK_DISABLE(AHBCLK_RTC0);
}
/*!\brief Clear the rtc flag
 *! \param none
 *! \retval none
 */
static void rtc_clear_flag(void)
{
    SAFE_CLK_CODE (
        GSP_RTC.ICR.Value = 0x01;
        GSP_RTC.WKUPCLR = 0x01;
    )
}
/*!\brief get the rtc flag
 *! \param none
 *! \retval none
 */
static bool rtc_get_flag(void)
{
    bool bResult;
    SAFE_CLK_CODE(
        bResult = GSP_RTC.WKUPSTS;
    )
    return bResult;
}
/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
static uint32_t rtc_get_time_value(void)
{
    uint32_t wResult;
    SAFE_CLK_CODE(
        wResult = GSP_RTC.DATA;
    )
    return wResult;
}
/*!\brief set count value
 *! \param void
 *! \retval return geh time count value
 */
static bool rtc_set_time_value(uint32_t wValue)
{
    SAFE_CLK_CODE (
        GSP_RTC.LOAD = wValue;
    )
    return true;
}

/*!\brief Set match value
 *! \param time match value
 *! \retval none
 */
static bool rtc_set_match_value(uint32_t wValue)
{
    SAFE_CLK_CODE(
        
        GSP_RTC.MATCH =  wValue;
    )
    return true;
}

/*!\brief get match value
 *! \param void
 *! \retval return match value
 */
static uint32_t rtc_read_match_value(void)
{
    uint32_t wResult;
    SAFE_CLK_CODE(
        wResult = GSP_RTC.MATCH;
    )
    return wResult;
}