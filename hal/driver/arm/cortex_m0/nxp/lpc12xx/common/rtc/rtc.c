/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\i_io_rtc.h"
#include "..\scon\pm.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define SAFE_CLK_CODE(...)                                                  \
    {                                                                       \
        uint32_t tAHBStatus = PM_AHB_CLK_GET_STATUS(AHBCLK_RTC);            \
        PM_AHB_CLK_ENABLE(AHBCLK_RTC);                                      \
        __VA_ARGS__;                                                        \
        PM_AHB_CLK_RESUME(AHBCLK_RTC,tAHBStatus);                           \
    }

/*============================ TYPES =========================================*/
//! \name RTC initialization arguments defination
//! @{
typedef enum {
    DISABLE_RTC             = 0x00,     //! Enable the RTC
    ENABLE_RTC              = 0x01,     //! Enable the RTC
    RTC_SRC_32KOSC_1HZ      = 0x00,     //! Select 1HZ 32KOSC output
    RTC_SRC_32KOSC_1KHZ     = 0x0Au << 1,     //! Select 1KHz 32KOSC output
    RTC_SRC_RTC_PCLK        = 0x04u << 1,     //! Select RTC PLCK clock source
    RTC_INTERRUPT_DISABLE   = 0x00,     //! Enable the RTC interrupt
    RTC_INTERRUPT_ENABLE    = 0x80,     //! Enable the RTC interrupt
} em_rtc_cfg_mode_t;
//! @}

//! \name RTC config struct
//! @{
typedef struct {
    uint8_t         chMode;                      //!< Watchdog config ward
    uint32_t        wMatchValue;
}rtc_cfg_t;
//! @}

DEF_INTERFACE(i_rtc_count_t)
    bool        (*Set)(uint32_t wValue);
    uint32_t    (*Get)(void);
END_DEF_INTERFACE(i_rtc_count_t)

DEF_INTERFACE(i_rtc_match_t)
    bool        (*Set)(uint32_t wValue);
    uint32_t    (*Get)(void);
END_DEF_INTERFACE(i_rtc_match_t)

//! \name rtc property type
//! @{
DEF_INTERFACE(i_rtc_flag_t)
    bool        (*Get)(void);
    void        (*Clear)(void);  
END_DEF_INTERFACE(i_rtc_flag_t)
//! @}


//! \name rtc struct
//! @{
DEF_INTERFACE(i_rtc_t)
    bool                (*Config)(rtc_cfg_t *ptCfg);   //!< initialize the RTC
    bool                (*Enable)(void);            //!< enable the ahbclk
    bool                (*Disable)(void);           //!< disable the ahbclk
    i_rtc_count_t       Count;        //!<  count value
    i_rtc_match_t       Match;                 //!<  match value
    i_rtc_flag_t        Flag;                       //!<  interrupt flag
END_DEF_INTERFACE(i_rtc_t)
//! @}

/*============================ PROTOTYPES ====================================*/
static bool rtc_cfg(rtc_cfg_t *tCfg);
static bool rtc_enable(void);
static bool rtc_disable(void);
static void rtc_clear_flag(void);
static bool rtc_get_flag(void);
static uint32_t rtc_get_time_value(void);
static bool     rtc_set_time_value(uint32_t wValue);
static uint32_t rtc_get_match_value(void);
static bool     rtc_set_match_value(uint32_t wValue);

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the RTC
const i_rtc_t RTC = {    
    &rtc_cfg,                       //!< initialize the RTC
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
    {
        &rtc_get_flag,
        &rtc_clear_flag,                        //!< clear interrupt flag
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief init real-time clock
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
static bool rtc_cfg(rtc_cfg_t *tCfg)
{
    PM_AHB_CLK_DISABLE(AHBCLK_RTC);
    PMU_REG.SYSCFG.RTCCLK = (tCfg->chMode >> 1) & 0x0Fu;
    SAFE_CLK_CODE (
        RTC_REG.CR.Value    = tCfg->chMode & 0x01u;
        RTC_REG.IMSC.Value  = tCfg->chMode >> 7;
        RTC_REG.MATCH       = tCfg->wMatchValue;
        RTC_REG.LOAD        = 0;
    )

    return true;
}

/*! \brief enable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static bool rtc_enable(void)
{
    PM_AHB_CLK_ENABLE(AHBCLK_RTC);
    
    return true;
}

/*! \brief disable the rtc AHBCLK
 *! \param none
 *! \retval none
 */
static bool rtc_disable(void)
{
    PM_AHB_CLK_DISABLE(AHBCLK_RTC);
    
    return true;
}

/*! \brief Clear the rtc flag
 *! \param none
 *! \retval none
 */
static void rtc_clear_flag(void)
{
    SAFE_CLK_CODE (
        RTC_REG.ICR.Value = 0x01u;
    )
}

/*! \brief get the rtc flag
 *! \param none
 *! \retval none
 */
static bool rtc_get_flag(void)
{
    bool bResult;
    
    SAFE_CLK_CODE(
        bResult = RTC_REG.RIS.RTCRIS;
    )
        
    return bResult;
}

/*! \brief get count value
 *! \param void
 *! \retval return geh time count value
 */
static uint32_t rtc_get_time_value(void)
{
    uint32_t wResult;
    
    SAFE_CLK_CODE(
        wResult = RTC_REG.DATA;
    )
        
    return wResult;
}

/*! \brief set count value
 *! \param void
 *! \retval return geh time count value
 */
static bool rtc_set_time_value(uint32_t wValue)
{
    SAFE_CLK_CODE (
        RTC_REG.LOAD = wValue;
    )
        
    return true;
}

/*! \brief Set match value
 *! \param time match value
 *! \retval none
 */
static bool rtc_set_match_value(uint32_t wValue)
{
    SAFE_CLK_CODE(
        RTC_REG.MATCH = wValue;
    )
        
    return true;
}

/*! \brief get match value
 *! \param void
 *! \retval return match value
 */
static uint32_t rtc_get_match_value(void)
{
    uint32_t wResult;
    
    SAFE_CLK_CODE(
        wResult = RTC_REG.MATCH;
    )
        
    return wResult;
}
