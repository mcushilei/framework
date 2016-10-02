/***************************************************************************
 *   Copyright(C)2009-2013 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
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

#ifndef __LPC_IO_H__
#define __LPC_IO_H__
#ifndef __LPC_IO_C__


/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"
#include ".\i_io_gpio.h"
#include ".\i_io_ioctrl.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __GPIO_INTERFACE(__N, __VALUE)      i_gpio_t GPIO##__N;

#define __IO_PORT_PIN_MSK(__N, __OFFSET)    PIN##__N##_MSK = (1ul << (__N)),

//! \brief enable pull-up
#define IO_ENABLE_PULL_UP(__PIN)        do {                                \
            IOCTRL_REG.PIN[(__PIN)].PUE = 1;                                \
        } while(0)

//! \brief disable pull-up
#define IO_DISABLE_PULL_UP(__PIN)       do {                                \
            IOCTRL_REG.PIN[(__PIN)].PUE = 0;                                \
        } while(0)

//! \brief io function selection
#define IO_FUNCTION_SELECT(__PIN, __FUNC)   do {                            \
            IOCTRL_REG.PIN[(__PIN)].FUNSEL = (__FUNC);                      \
        } while(0)

#define IO_CFG(...)                     do {                            \
            io_cfg_t tCFG[] = {__VA_ARGS__};                            \
            uint32_t wSize = 0;                                         \
            for (; wSize != UBOUND(tCFG); ++wSize) {                    \
                uint32_t wPinIndex  = tCFG[wSize].tPIN;                 \
                uint32_t wIOCTRL    = tCFG[wSize].wMode ^ IOCTRL_PIN_ADMOD_MSK \
                                    | IOCTRL_PIN_FUNSEL(tCFG[wSize].wFunction);\
                if (wPinIndex >= IO_PIN_COUNT) {                        \
                    break;                                              \
                }                                                       \
                IOCTRL_REG.PIN[wPinIndex].Value = wIOCTRL;              \
            }                                                           \
        } while(0)

/*============================ TYPES =========================================*/
//! \name pin mask
//! @{
typedef enum {
    MREPEAT(IO_PORT_PIN_COUNT, __IO_PORT_PIN_MSK, 0)
} em_io_pin_msk_t;
//! @}

//! \name Port No.
//! @{
typedef enum {
#if defined(IO_PORTA)
    PA,
#endif
#if defined(IO_PORTB)
    PB,
#endif
#if defined(IO_PORTC)
    PC,
#endif
#if defined(IO_PORTD)
    PD,
#endif
} em_io_port_no_t;
//! @}

//! \name io configuration structure
//! @{
typedef struct {
    em_io_pin_t     tPIN;           //!< pin number
    uint32_t        wFunction;      //!< io Funcitons
    uint32_t        wMode;          //!< io mode
} io_cfg_t;
//! @}

//! \name gpio control interface
//! @{
DEF_INTERFACE(i_gpio_t)
    void        (*SetDirection)(uint32_t wDirection, uint32_t wPinMask);
    uint32_t    (*GetDirection)(uint32_t wPinMask);
    void        (*SetInput)(uint32_t wPinMask);
    void        (*SetOutput)(uint32_t wPinMask);
    void        (*SwitchDirection)(uint32_t wPinMask);
    uint32_t    (*Read)(void);
    void        (*Write)(uint32_t wValue, uint32_t wPinMask);
    void        (*Set)(uint32_t wPinMask);
    void        (*Clear)(uint32_t wPinMask);
    void        (*Toggle)(uint32_t wPinMask);
END_DEF_INTERFACE(i_gpio_t)
//! @}

//! \name csc user interface
//! @{
DEF_INTERFACE(io_t)
    bool            (*Config)(io_cfg_t const *ptCFG, uint32_t wSize);//!< io configuration
    union {
        i_gpio_t  GPIO[IO_PORT_COUNT];               //!< dedicated gpio control interface
        struct {
            MREPEAT(IO_PORT_COUNT, __GPIO_INTERFACE, 0)
        };
    };
END_DEF_INTERFACE(io_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief io interface
extern const io_t IO;

/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
