#ifndef __RTC_H__
#define __RTC_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\i_io_rtc.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define RTC_CFG(...)                        \
    do{                                     \
        rtc_cfg_t tCfg = { __VA_ARGS__ };   \
        RTC.Init(&tCfg);                    \
    }while(false)

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
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the PMU
extern const rtc_t RTC;
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
