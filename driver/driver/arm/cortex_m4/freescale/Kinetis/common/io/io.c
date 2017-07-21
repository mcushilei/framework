
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


//! \brief function selection
#define IOCTRL_FUNCTION_SELECT(__PIN, __FUNC)   do {                            \
            ioctrl_reg_t *ptPort = &GSP_IOCTRL0 + ((__PIN) >> 5);               \
            ptPort->PIN[(__PIN) & 0x1F].FSEL = (__FUNC);                        \
        } while(0)

#define __GPIO_FUNC_DEF(__N,__VALUE)                                            \
static void gpio##__N##_set_direction(uint32_t wDirection, uint32_t wPinMask);  \
static uint32_t    gpio##__N##_get_direction(uint32_t wPinMask);                \
static void        gpio##__N##_set_input(uint32_t wPinMask);                    \
static void        gpio##__N##_set_output(uint32_t wPinMask);                   \
static void        gpio##__N##_switch_direction(uint32_t wPinMask);             \
static uint32_t    gpio##__N##_read(void);                                      \
static void        gpio##__N##_write(uint32_t wValue, uint32_t wPinMask);       \
static void        gpio##__N##_set(uint32_t wPinMask);                          \
static void        gpio##__N##_clear(uint32_t wPinMask);                        \
static void        gpio##__N##_toggle(uint32_t wPinMask);                       

#define __GPIO_INTERFACE_DEF(__N, __VALUE)                                      \
            {                                                                   \
                                                                                \
                &gpio##__N##_set_direction,                                     \
                &gpio##__N##_get_direction,                                     \
                &gpio##__N##_set_input,                                         \
                &gpio##__N##_set_output,                                        \
                &gpio##__N##_switch_direction,                                  \
                &gpio##__N##_read,                                              \
                &gpio##__N##_write,                                             \
                &gpio##__N##_set,                                               \
                &gpio##__N##_clear,                                             \
                &gpio##__N##_toggle,                                            \
                ( gpio_reg_t *)GPIO##__N##_BASE_ADDRESS,                        \
            },

#define __GPIO_FUNC_BODY(__N, __VALUE)                                          \
static void gpio##__N##_set_direction(uint32_t wDirection, uint32_t wPinMask)   \
{                                                                               \
    SAFE_ATOM_CODE (                                                            \
        uint32_t wTemp = GSP_GPIO##__N.DIR & ~wPinMask;                         \
        wTemp |= (wDirection & wPinMask);                                       \
        GSP_GPIO##__N.DIR = wTemp;                                              \
    )                                                                           \
}                                                                               \
static uint32_t gpio##__N##_get_direction(uint32_t wPinMask)                    \
{                                                                               \
    return GSP_GPIO##__N.DIR;                                                   \
}                                                                               \
static void gpio##__N##_set_input(uint32_t wPinMask)                            \
{                                                                               \
    GSP_GPIO##__N.DIR &= ~wPinMask;                                             \
}                                                                               \
static void gpio##__N##_set_output(uint32_t wPinMask)                           \
{                                                                               \
    GSP_GPIO##__N.DIR |= wPinMask;                                              \
}                                                                               \
static void gpio##__N##_switch_direction(uint32_t wPinMask)                     \
{                                                                               \
    GSP_GPIO##__N.DIR ^= wPinMask;                                              \
}                                                                               \
static uint32_t gpio##__N##_read(void)                                          \
{                                                                               \
    return GSP_GPIO##__N.IN;                                                    \
}                                                                               \
static void gpio##__N##_write(uint32_t wValue, uint32_t wPinMask)               \
{                                                                               \
    SAFE_ATOM_CODE (                                                            \
        uint32_t wTemp = GSP_GPIO##__N.OUT & ~wPinMask;                         \
        wTemp |= (wValue & wPinMask);                                           \
        GSP_GPIO##__N.OUT = wTemp;                                              \
    )                                                                           \
                                                                                \
}                                                                               \
static void gpio##__N##_set(uint32_t wPinMask)                                  \
{                                                                               \
    GSP_GPIO##__N.OUTSET = wPinMask;                                            \
}                                                                               \
static void gpio##__N##_clear(uint32_t wPinMask)                                \
{                                                                               \
    GSP_GPIO##__N.OUTCLR = wPinMask;                                            \
}                                                                               \
static void gpio##__N##_toggle(uint32_t wPinMask)                               \
{                                                                               \
    GSP_GPIO##__N.OUTTOG = wPinMask;                                            \
}                                                                                                                                             


/*============================ MACROFIED FUNCTIONS ===========================*/
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


 
//! \name pin mode
//! @{
typedef enum {
//! \name standard
//! @{
    PIN_DEFAULT                 = 0,
    PIN_INPUT                   = 0,            //!< input
    PIN_OUTPUT                  = 0x80,         //!< output    
    
    PIN_PULL_UP                 = 1,            //!< enable pull up resistor
    PIN_PULL_DOWN               = 2,            //!< enable pull down resistor
    PIN_OPEN_DRAIN              = 3,            //!< open drain mode
    PIN_BUS_KEEP                = 4,            //!< enable bus keep mode
//! @}
    PIN_SLOW_SLEW_RATE          = 5,            //!< slow slew rate
    PIN_PASSIVE_FILTER          = 6,            //!< passive filter
    PIN_HIGH_DRIVE_STRENGTH     = 7,            //!< high drive strength
}pin_mode_t;
//! @}

//! \name pin configuration
//! @{
typedef struct {
    uint8_t         chPort;         //!< port number 
    uint32_t        wPinMask;       //!< pin index
    uint8_t         chMux;          //!< pin mux
    pin_mode_t      tMode;          //!< pin mode
} pin_cfg_t;
//! @}

//! \name pin function selection
//! @{
typedef enum {
    PIN_FUNC_DEFAULT            = 0,
    PIN_FUNC_ANALOG             = 0,
    PIN_FUNC_GPIO               = 1,
    PIN_FUNC_ALTERNATE_A        = 2,
    PIN_FUNC_ALTERNATE_B        = 3,
    PIN_FUNC_ALTERNATE_C        = 4,
    PIN_FUNC_ALTERNATE_D        = 5,
    PIN_FUNC_ALTERNATE_E        = 6,
    PIN_FUNC_ALTERNATE_F        = 7
}pin_func_t;
//! @}


/*============================ PROTOTYPES ====================================*/
static bool io_configuration( io_cfg_t *ptCFG, uint_fast8_t chSize );
MREPEAT(GPIO_COUNT, __GPIO_FUNC_DEF, NULL)
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
//! \brief io interface
const io_t IO = {
        &io_configuration,                  //!< general io configuration
        {
            {MREPEAT(GPIO_COUNT, __GPIO_INTERFACE_DEF, NULL)}
        }
    };

/*============================ IMPLEMENTATION ================================*/

MREPEAT(GPIO_COUNT, __GPIO_FUNC_BODY, NULL)

/*! \name io general configuration
 *! \param io configuration structure array
 *! \param chSize array size
 *! \retval true configuration succeed
 *! \retval false configuration fail
 */
static bool io_configuration( io_cfg_t *ptCFG, uint_fast8_t chSize )
{
    bool tResult = true;
    if (NULL == ptCFG || 0 == chSize) {
        return false;
    }

    //! io configure
    do {
        uint_fast8_t chPinIndex = ptCFG->tPIN;
        uint32_t wFunction = ptCFG->wFunction;
        uint32_t wIOCTRL = ptCFG->wMode;
        
        ioctrl_reg_t *ptPort = &GSP_IOCTRL0 + (chPinIndex >> 5);
        chPinIndex &= 0x1F;
        ptPort->PIN[chPinIndex].Value = wIOCTRL;
        ptPort->PIN[chPinIndex].FSEL = wFunction;
        ptCFG++;
    } while(--chSize);

    return tResult;
}

///*! \brief configure multiple-pins in the specified port
// *! \param chIndex port index
// *! \param wPinMask pin mask
// *! \param chPinMux pin functions selection
// *! \param tMode pin work mode
// */
//gsf_err_t k20_gpio_pin_config( uint_fast8_t chIndex, 
//    uint32_t wPinMask, uint_fast8_t chPinMux, pin_mode_t tMode)
//{
//    uint_fast8_t n = 32;
//    uint_fast8_t chBit = 0;
//    PORT_MemMapPtr tPort;
//
//    if (chIndex >= GPIO_COUNT) {
//        return GSF_ERR_INVALID_RANGE;
//    }
//
//    tPort = (PORT_MemMapPtr)&PORT[chIndex];
//    do {
//        if (wPinMask & 0x01) {
//            uint32_t tTemp = tPort->PCR[chBit];
//            //! set pin mux
//            tTemp &= ~( PORT_PCR_MUX_MASK   | 
//                        PORT_PCR_ODE_MASK   | 
//                        PORT_PCR_PE_MASK    |
//                        PORT_PCR_PS_MASK    |
//                        PORT_PCR_SRE_MASK   |
//                        PORT_PCR_DSE_MASK   |
//                        PORT_PCR_PFE_MASK
//                      );
//            tTemp |= (chPinMux << PORT_PCR_MUX_SHIFT);
//            switch (tMode & 0x03) {
//                case PIN_PULL_UP:
//                    tTemp |= PORT_PCR_PS_MASK;
//                    /* please do not add BREAK here*/
//                case PIN_PULL_DOWN:
//                    tTemp |= PORT_PCR_PE_MASK;
//                    break;
//                case PIN_BUS_KEEP:
//                    /* no implemented */
//                case PIN_OPEN_DRAIN:
//                    tTemp |= PORT_PCR_ODE_MASK;
//                    break;
//                default:
//                    break;
//            }
//            //! slow slew rate
//            if ((tMode & PIN_SLOW_SLEW_RATE) == PIN_SLOW_SLEW_RATE) {
//                tTemp |= PORT_PCR_SRE_MASK;
//            }
//            //! passive filter
//            if ((tMode & PIN_PASSIVE_FILTER) == PIN_PASSIVE_FILTER) {
//                tTemp |= PORT_PCR_PFE_MASK;
//            }
//            //! high drive strenght
//            if ((tMode & PIN_HIGH_DRIVE_STRENGTH) == PIN_HIGH_DRIVE_STRENGTH) {
//                tTemp |= PORT_PCR_DSE_MASK;
//            }
//            //! input/output
//            if (tMode & PIN_OUTPUT) {
//                GPIO[chIndex].DIR |= _BV(chBit);
//            } else {
//                GPIO[chIndex].DIR &= ~_BV(chBit);
//            }
//            tPort->PCR[chBit] = tTemp;
//        }
//        wPinMask >>= 1;
//        chBit++;
//    } while(--n);
//
//    return GSF_ERR_NONE;
//}

/* EOF */

