#ifndef __RTC_H__
#define __RTC_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\i_io_rtc.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define RTC_CFG(...)    do{                 \
        rtc_cfg_t tCfg = { __VA_ARGS__ };   \
        RTC.Config(&tCfg);                    \
    } while(false)

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
    bool                (*Config)(rtc_cfg_t *tCfg);   //!< initialize the RTC
    bool                (*Enable)(void);            //!< enable the ahbclk
    bool                (*Disable)(void);           //!< disable the ahbclk
    i_rtc_count_t       Count;        //!<  count value
    i_rtc_match_t       Match;                 //!<  match value
    i_rtc_flag_t        Flag;                       //!<  interrupt flag
END_DEF_INTERFACE(i_rtc_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the RTC
extern const i_rtc_t RTC;

/*============================ PROTOTYPES ====================================*/

#endif
