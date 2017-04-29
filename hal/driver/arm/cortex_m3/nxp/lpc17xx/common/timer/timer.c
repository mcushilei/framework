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
#define __DRIVER_TIMER_C__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\reg_timer.h"
#include "..\scon\pm.h"

/*============================ MACROS ========================================*/
//! \brief timer channel max counter
#define TC_MAX_CHANNEL          (2ul)


#define this        (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/   

//! \brief safe clock code 
#define SAFE_CLK_CODE(...)                                                      \
    {                                                                           \
        uint32_t tStatus = peripheral_clock_get_status(this.tPCON);             \
        peripheral_clock_enable(this.tPCON);                                    \
        __VA_ARGS__;                                                            \
        peripheral_clock_resume_status(this.tPCON, tStatus);                    \
    }

#define __TIMER_OBJ(__N, __A)                                                \
    {                                                                       \
        ((tmr_reg_t *)(TIM##__N##_BASE_ADDRESS)),                           \
        PCONP_TIM##__N,                                                    \
        PCLK_TIM##__N,                                                     \
    },

#define __TIMER_FUNCTION(__N, __A)                                              \
    bool timer##__N##_init(void)                                                \
    {                                                                           \
        return timer_init(&__TIMER[__N]);                                       \
    }                                                                           \
    bool timer##__N##_enable(void)  \
    {                                   \
        return timer_enable(&__TIMER[__N]);    \
    }                                           \
    bool timer##__N##_disable(void) \
    {                                   \
        return timer_disable(&__TIMER[__N]);    \
    }                                           \
    bool timer##__N##_start(void)   \
    {                                               \
        return timer_start(&__TIMER[__N]);         \
    }                                           \
    bool timer##__N##_stop(void)                            \
    {                                       \
        return timer_stop(&__TIMER[__N]);              \
    }                                           \

/*============================ TYPES =========================================*/
//! \name internal class
//! @{
typedef struct {
    tmr_reg_t * const   ptReg;              //!< reference to register page
    const uint32_t      tPCON;              //!< ahbclk info
    const uint32_t      tPCLK;              //!<
} __timer_t;
//! @}

/*============================ PROTOTYPES ====================================*/
static bool timer_init(__timer_t *ptThis);
static bool timer_enable(__timer_t *ptThis);
static bool timer_disable(__timer_t *ptThis);
static bool timer_start(__timer_t *ptThis);
static bool timer_stop(__timer_t *ptThis);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
//! \brief internal timer object
static __timer_t __TIMER[] = {
    MREPEAT(TIMER_COUNT, __TIMER_OBJ, NULL)
};

/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize timer
 *! \param ptThis timer object
 *! \param ptTimerCfg timer configuration object
 *! \retval true initialization succeed
 *! \retval false initialization failed
 */
static bool timer_init(__timer_t *ptThis)
{
    return true;
}

/*! \brief timer enable
 *! \param ptThis timer object
 *! \retval true timer enable succeed
 *! \retval false timer enable failed
 */
static bool timer_enable(__timer_t *ptThis)
{
    if (NULL == ptThis) {
        return false;
    }

    //! Enable AHB Clock
    peripheral_clock_enable(this.tPCON);

    return true;
}

/*! \brief timer disable
 *! \param ptThis timer object
 *! \retval true timer disable succeed
 *! \retval false timer disable failed
 */
static bool timer_disable(__timer_t *ptThis)
{
    if (NULL == ptThis) {
        return false;
    }

    //! Disable AHB Clock
    peripheral_clock_disable(this.tPCON);
    
    return true;
}  

/*! \brief timer start count
 *! \param ptThis timer object
 *! \retval true timer start count succeed
 *! \retval false timer start count failed
 */
static bool timer_start(__timer_t *ptThis)
{
    if (NULL == ptThis) {
        return false;
    }

    SAFE_CLK_CODE(
        this.ptReg->TCR = (1u << 0);
    );

    return true;
}

/*! \brief timer stop count
 *! \param ptThis timer object
 *! \retval true timer stop count succeed
 *! \retval false timer stop count failed
 */
static bool timer_stop(__timer_t *ptThis)
{
    if (NULL == ptThis) {
        return false;
    }

    SAFE_CLK_CODE(
        this.ptReg->TCR = 0;
    );

    return true;
}    


//! function list
MREPEAT(TIMER_COUNT, __TIMER_FUNCTION, NULL)
            
