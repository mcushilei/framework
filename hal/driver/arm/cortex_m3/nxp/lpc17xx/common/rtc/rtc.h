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
    u32_property_t      Count;        //!<  count value
    u32_property_t      Match;                 //!<  match value
} rtc_t;
//! @}
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief declare the PMU
extern const rtc_t RTC;

/*============================ PROTOTYPES ====================================*/

#endif
