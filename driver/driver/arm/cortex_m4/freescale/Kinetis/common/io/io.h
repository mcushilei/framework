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

#ifndef __DRIVER_ARM_M4_FREESCALE_KINETIS_COMMON_IO_H__
#define __DRIVER_ARM_M4_FREESCALE_KINETIS_COMMON_IO_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\device.h"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
#define __GPIO_INTERFACE(__N, __VALUE)      const i_gpio_t GPIO##__N;


#define __IO_PINA_NUM(__N, __OFFSET)        PA##__N = (__OFFSET) + (__N),
#define __IO_PINB_NUM(__N, __OFFSET)        PB##__N = (__OFFSET) + (__N),
#define __IO_PINC_NUM(__N, __OFFSET)        PC##__N = (__OFFSET) + (__N),
#define __IO_PIND_NUM(__N, __OFFSET)        PD##__N = (__OFFSET) + (__N),
#define __IO_PINE_NUM(__N, __OFFSET)        PE##__N = (__OFFSET) + (__N),

#define __IO_PINA_MSK(__N, __OFFSET)        PA##__N##_MSK = (1ul<<(__N)),
#define __IO_PINB_MSK(__N, __OFFSET)        PB##__N##_MSK = (1ul<<(__N)),
#define __IO_PINC_MSK(__N, __OFFSET)        PC##__N##_MSK = (1ul<<(__N)),
#define __IO_PIND_MSK(__N, __OFFSET)        PD##__N##_MSK = (1ul<<(__N)),
#define __IO_PINE_MSK(__N, __OFFSET)        PE##__N##_MSK = (1ul<<(__N)),


#define IO_CFG(...)                                                             \
            do {                                                                \
                io_cfg_t tCFG[] = {__VA_ARGS__};                                \
                IO.Config(tCFG, UBOUND(tCFG));                                  \
            } while(0)

/*============================ TYPES =========================================*/

//! \name PIN name 
//! @{
typedef enum {
#if defined(PIO_PORTA)
    MREPEAT(PIO_PORTA_PIN_NUM, __IO_PINA_NUM, 0)
#endif
#if defined(PIO_PORTB)
    MREPEAT(PIO_PORTB_PIN_NUM, __IO_PINB_NUM, 32)
#endif
#if defined(PIO_PORTC)
    MREPEAT(PIO_PORTC_PIN_NUM, __IO_PINC_NUM, 64)
#endif
#if defined(PIO_PORTD)
    MREPEAT(PIO_PORTD_PIN_NUM, __IO_PIND_NUM, 96)
#endif
#if defined(PIO_PORTE)
    MREPEAT(PIO_PORTE_PIN_NUM, __IO_PINE_NUM, 128)
#endif
}em_io_pin_no_t;
//! @}

//! \name PIN name 
//! @{
typedef enum {
#if defined(PIO_PORTA)
    MREPEAT(PIO_PORTA_PIN_NUM, __IO_PINA_MSK, 0)
#endif
#if defined(PIO_PORTB)
    MREPEAT(PIO_PORTB_PIN_NUM, __IO_PINB_MSK, 0)
#endif
#if defined(PIO_PORTC)
    MREPEAT(PIO_PORTC_PIN_NUM, __IO_PINC_MSK, 0)
#endif
#if defined(PIO_PORTD)
    MREPEAT(PIO_PORTD_PIN_NUM, __IO_PIND_MSK, 0)
#endif
#if defined(PIO_PORTE)
    MREPEAT(PIO_PORTE_PIN_NUM, __IO_PINE_MSK, 0)
#endif
}em_io_pin_msk_t;
//! @}

//! \name Port name 
//! @{
typedef enum {
#if defined(PIO_PORTA)
    PA = 0,
#endif
#if defined(PIO_PORTB)
    PB = 1,
#endif
#if defined(PIO_PORTC)
    PC = 2,
#endif
#if defined(PIO_PORTD)
    PD = 3,
#endif
#if defined(PIO_PORTE)
    PE = 4,
#endif
}em_io_port_no_t;
//! @}

//! \name port

//! \name IO model
//! @{
typedef enum {
    IO_PULL_UP              = (3<<0),           //!< enable pull-up resistor
    IO_OPEN_DRAIN           = (1<<5),           //!< enable open-drain mode

    IO_HIGH_DRV             = (1<<6),           //!< enable high drive strength
    IO_HIGH_DRIVE           = (1<<6),           //!< enable high drive strength
    IO_HIGH_DRIVE_STRENGTH  = (1<<6),           //!< enable high drive strength
}em_io_model_t;
//! @}

//! \name io configuration structure
//! @{
typedef struct {
    em_io_pin_no_t  tPIN;                   //! pin number
    uint32_t        wFunction;              //!< io Funcitons
    uint32_t        wMode;
}io_cfg_t;
//! @}

//! \name gpio control interface
//! @{
DEF_INTERFACE(i_gpio_t)
    
    //! set pin directions with pin-mask
    void        (*SetDirection)(uint32_t wDirection, uint32_t wPinMask);
    //! get pin direction with pin-mask
    uint32_t    (*GetDirection)(uint32_t wPinMask);
    //! Set specified pin direction to input 
    void        (*SetInput)(uint32_t wPinMask);
    //! Set specified pin direction to output
    void        (*SetOutput)(uint32_t wPinMask);
    //! Switch specified pin direction
    void        (*SwitchDirection)(uint32_t wPinMask);
    //! get pin value on specified port
    uint32_t    (*Read)(void);
    //! write pin value with pin-mask
    void        (*Write)(uint32_t wValue, uint32_t wPinMask);
    //! set specified pins 
    void        (*Set)(uint32_t wPinMask);
    //! clear specified pins 
    void        (*Clear)(uint32_t wPinMask);
    //! toggle specified pins
    void        (*Toggle)(uint32_t wPinMask);
    //! get base address of specified port
    gpio_reg_t *const ptRegPage;

END_DEF_INTERFACE(i_gpio_t)
//! @}

//! \name csc user interface
//! @{
DEF_INTERFACE(io_t)
    //! general io configuration
    bool            (*Config)(io_cfg_t *ptCFG, uint_fast8_t chSize);
    union {
        const i_gpio_t  GPIO[GPIO_COUNT];               //!< dedicated gpio control interface
        struct {
            MREPEAT(GPIO_COUNT, __GPIO_INTERFACE, NULL)
        };
    };
END_DEF_INTERFACE(io_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief io interface
extern const io_t IO;

/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */