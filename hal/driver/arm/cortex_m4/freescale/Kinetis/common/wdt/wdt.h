#ifndef __WDT_H__
#define __WDT_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name watchdog initialization arguments defination
//! @{
typedef enum {
    //! A watchdog time-out will not cause reset
    WDT_WDRESET_INT             = 0x00,
    //! A watchdog time-out will cause reset
    WDT_WDRESET_RESET           = 0x02,    
    
    //! change time-out value(TC) at any time
    WDT_CHANGE_TC_ANYTIME       = 0x00,
    //! change time-out value(TC) only after counter is below WDWARNINT and WDWINDOW
    WDT_CHANGE_TC_BELOW_W       = 0x10,

    //! change watchdog oscillator at any time
    WDT_OSC_IS_NOT_LOCKED       = 0x00,
    //! watchdog oscillator is locked
    WDT_OSC_IS_LOCKED           = 0x20,

} em_wdt_cfg_mode_t;
//! @}


//! \name wdt config struct
//! @{
typedef union {
    struct {
        uint32_t    wMode;                          //!< Watchdog config ward
        uint32_t    wTCCount            : 24;       //!< Watchdog time-out value.
        uint32_t                        : 8;
        uint32_t    wWarnInterCmpValue  : 10;       //!< Watchdog warning interrupt compare value.
        uint32_t                        : 22;
        uint32_t    wWindowCmpValue     : 24;       //!< Watchdog window value.
        uint32_t                        : 8;
    }_;
    uint32_t wValue[4];
}wdt_cfg_t;
//! @}

//! \name wdt struct
//! @{
typedef struct {
    bool        (*Init)(wdt_cfg_t *tCfg);           //!< initialize the wdt
    void        (*Enable)(void);                    //!< enable the watchdog
    void        (*Disable)(void);                   //!< disable the watchdog
    void        (*Reset)(void);                     //!< reset the watchdog
    uint32_t    (*GetCountValue)(void);             //!< get the time count value
} wdt_t;
//! @}
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the PMU
extern const wdt_t WDT;


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
