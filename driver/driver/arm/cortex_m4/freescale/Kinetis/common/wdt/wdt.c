
/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
static void watchdog_unlock(void);
static void watchdog_disable(void);
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief define the WDT
const wdt_t WDT = {
    .Disable = &watchdog_disable
};

/*============================ IMPLEMENTATION ================================*/

/*! \brief Watchdog timer disable routine
 *! \param none
 *! \return none
 */
static void watchdog_disable(void)
{
	/* First unlock the watchdog so that we can write to registers */
	watchdog_unlock();
	
	/* Clear the WDOGEN bit to disable the watchdog */
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/*
 * Watchdog timer unlock routine. Writing 0xC520 followed by 0xD928
 * will unlock the write once registers in the WDOG so they are writable
 * within the WCT period.
 *
 * Parameters:
 * none
 */
static void watchdog_unlock(void)
{
  /* NOTE: DO NOT SINGLE STEP THROUGH THIS FUNCTION!!! */
  /* There are timing requirements for the execution of the unlock. If
   * you single step through the code you will cause the CPU to reset.
   */

    /* This sequence must execute within 20 clock cycles, so disable
     * interrupts will keep the code atomic and ensure the timing.
     */
    SAFE_ATOM_CODE(	
        /* Write 0xC520 to the unlock register */
        WDOG_UNLOCK = 0xC520;
        
        /* Followed by 0xD928 to complete the unlock */
        WDOG_UNLOCK = 0xD928;	
    )
}

/* EOF */