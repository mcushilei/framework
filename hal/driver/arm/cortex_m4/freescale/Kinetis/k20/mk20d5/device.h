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


#ifndef __USE_FREESCALE_MK20D5_DEVICE_H__
#define __USE_FREESCALE_MK20D5_DEVICE_H__


/*============================ INCLUDES ======================================*/
#include <freescale\MK20D5.h>
#undef SCB_AIRCR_SYSRESETREQ_MASK
#undef SYST_CSR
#undef SYST_RVR
#undef SYST_CVR
#undef SYST_CALIB
#include ".\vectors.h"

/*============================ MACROS ========================================*/

#ifndef __HZ_TYPE__
#define __HZ_TYPE__
#define MHz                     *1000000l
#define KHz                     *1000l
#define Hz                      *1l
#endif

#ifndef DEF_REG
#define DEF_REG                     \
        union  {                    \
            struct {
#endif
    
#ifndef END_DEF_REG
#define END_DEF_REG(__NAME)         \
            };                      \
            reg32_t Value;          \
        }__NAME;
#endif

#ifndef REG_RSVD_0x10
#define REG_RSVD_0x10                   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   
#endif
#ifndef REG_RSVD_0x80       
#define REG_RSVD_0x80                   \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10
#endif

#ifndef REG_RSVD_0x100                 
#define REG_RSVD_0x100                  \
            REG_RSVD_0x80               \
            REG_RSVD_0x80
#endif

#ifndef REG_RSVD_0x800
#define REG_RSVD_0x800                  \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100
#endif

/*----------------------------------------------------------------------------*
 * GPIO Device Dependent Specification                                        *
 *----------------------------------------------------------------------------*/
#define GPIO_COUNT              5

//! \name gpio macros
//! @{
#define GPIO0_BASE_ADDRESS              0x400FF000u
#define GSP_GPIO_PORTA                  (*(gpio_reg_t *)GPIO0_BASE_ADDRESS)
#define GSP_PORTA                       (*(gpio_reg_t *)GPIO0_BASE_ADDRESS)
#define GSP_GPIO0                       (*(gpio_reg_t *)GPIO0_BASE_ADDRESS)
#define GPIO_PORTA_PIN_COUNT            32

#define GPIO1_BASE_ADDRESS              0x400FF040u    
#define GSP_GPIO_PORTB                  (*(gpio_reg_t *)GPIO1_BASE_ADDRESS)
#define GSP_PORTB                       (*(gpio_reg_t *)GPIO1_BASE_ADDRESS)
#define GSP_GPIO1                       (*(gpio_reg_t *)GPIO1_BASE_ADDRESS)
#define GPIO_PORTB_PIN_COUNT            32

#define GPIO2_BASE_ADDRESS              0x400FF080u
#define GSP_GPIO_PORTC                  (*(gpio_reg_t *)GPIO2_BASE_ADDRESS)
#define GSP_PORTC                       (*(gpio_reg_t *)GPIO2_BASE_ADDRESS)
#define GSP_GPIO2                       (*(gpio_reg_t *)GPIO2_BASE_ADDRESS)
#define GPIO_PORTC_PIN_COUNT            32

#define GPIO3_BASE_ADDRESS              0x400FF0C0u    
#define GSP_GPIO_PORTD                  (*(gpio_reg_t *)GPIO3_BASE_ADDRESS)
#define GSP_PORTD                       (*(gpio_reg_t *)GPIO3_BASE_ADDRESS)
#define GSP_GPIO3                       (*(gpio_reg_t *)GPIO3_BASE_ADDRESS)
#define GPIO_PORTD_PIN_COUNT            32

#define GPIO4_BASE_ADDRESS              0x400FF100u    
#define GSP_GPIO_PORTE                  (*(gpio_reg_t *)GPIO4_BASE_ADDRESS)
#define GSP_PORTE                       (*(gpio_reg_t *)GPIO4_BASE_ADDRESS)
#define GSP_GPIO4                       (*(gpio_reg_t *)GPIO4_BASE_ADDRESS)
#define GPIO_PORTE_PIN_COUNT            32

//! @}

//! \name IO Controller macros
//! @{
#define IOCTRL0_BASE_ADDRESS            0x40049000u
#define GSP_IOCTRL0                     ( *(ioctrl_reg_t *)IOCTRL0_BASE_ADDRESS )
#define PIO_PORTA
#define PIO_PORTA_PIN_NUM               32


#define IOCTRL1_BASE_ADDRESS            0x4004A000u
#define GSP_IOCTRL1                     ( *(ioctrl_reg_t *)IOCTRL1_BASE_ADDRESS )
#define PIO_PORTB
#define PIO_PORTB_PIN_NUM               32


#define IOCTRL2_BASE_ADDRESS            0x4004B000u
#define GSP_IOCTRL2                     ( *(ioctrl_reg_t *)IOCTRL2_BASE_ADDRESS )
#define PIO_PORTC
#define PIO_PORTC_PIN_NUM               32


#define IOCTRL3_BASE_ADDRESS            0x4004C000u
#define GSP_IOCTRL3                     ( *(ioctrl_reg_t *)IOCTRL3_BASE_ADDRESS )
#define PIO_PORTD
#define PIO_PORTD_PIN_NUM               32

#define IOCTRL4_BASE_ADDRESS            0x4004D000u
#define GSP_IOCTRL4                     ( *(ioctrl_reg_t *)IOCTRL4_BASE_ADDRESS )
#define PIO_PORTE
#define PIO_PORTE_PIN_NUM               32
//! @}


//! \name port power-gating
//! @{
#define SIM_SCGC_PORT           SIM_SCGC5
#define SIM_SCGC_PORTA_MSK      SIM_SCGC5_PORTA_MASK
#define SIM_SCGC_PORTA_SHIFT    SIM_SCGC5_PORTA_SHIFT
#define SIM_SCGC_PORTB_MSK      SIM_SCGC5_PORTB_MASK
#define SIM_SCGC_PORTB_SHIFT    SIM_SCGC5_PORTB_SHIFT
#define SIM_SCGC_PORTC_MSK      SIM_SCGC5_PORTC_MASK
#define SIM_SCGC_PORTC_SHIFT    SIM_SCGC5_PORTC_SHIFT
#define SIM_SCGC_PORTD_MSK      SIM_SCGC5_PORTD_MASK
#define SIM_SCGC_PORTD_SHIFT    SIM_SCGC5_PORTD_SHIFT
#define SIM_SCGC_PORTE_MSK      SIM_SCGC5_PORTE_MASK
#define SIM_SCGC_PORTE_SHIFT    SIM_SCGC5_PORTE_SHIFT
//! @}

/*----------------------------------------------------------------------------*
 * USART Device Dependent Specification                                       *
 *----------------------------------------------------------------------------*/

//! \name usart
//! @{
#define USART_COUNT                     3

#define USART0_BASE_ADDRESS             (0x4006A000ul)
#define USART0                          USART[0]
#define GSP_USART0                      (*(usart_reg_t *)USART0_BASE_ADDRESS)

#define USART1_BASE_ADDRESS             (0x4006B000ul)
#define USART1                          USART[1]
#define GSP_USART1                      (*(usart_reg_t *)USART1_BASE_ADDRESS)

#define USART2_BASE_ADDRESS             (0x4006C000ul)
#define USART2                          USART[2]
#define GSP_USART2                      (*(usart_reg_t *)USART2_BASE_ADDRESS)
//! @}

//! \for debug purpose
extern volatile uint32_t core_clk_khz;
extern volatile int32_t  periph_clk_khz; 

//! for debug purpose
#define USART_GET_P_CLK()                   (periph_clk_khz * 1000)
#define USART_GET_CORE_CLK()                (core_clk_khz * 1000)
#define PM_GET_SYS_CLK()                    (core_clk_khz * 1000)
//! \name usart power-gating
//! @{
#define SIM_SCGC_UART           SIM_SCGC4
#define SIM_SCGC_UART0_MASK      SIM_SCGC4_UART0_MASK
#define SIM_SCGC_UART0_SHIFT    SIM_SCGC4_UART0_SHIFT
#define SIM_SCGC_UART1_MASK      SIM_SCGC4_UART1_MASK
#define SIM_SCGC_UART1_SHIFT    SIM_SCGC4_UART1_SHIFT
#define SIM_SCGC_UART2_MASK      SIM_SCGC4_UART2_MASK
#define SIM_SCGC_UART2_SHIFT    SIM_SCGC4_UART2_SHIFT
//! @}

/*----------------------------------------------------------------------------*
 * SPI Device Dependent Specification                                         *
 *----------------------------------------------------------------------------*/
#define GSF_SPI_COUNT          1
#define GSF_SPI0               0

#define SPI_BASE_ADDRESS        (0x4002C000u)   
#define SPI_MAX_ENTRY_COUNT     4

#define SPI0                    SPI[GSF_SPI0]

#define SPI   (*(k20_spi_array_t *)SPI_BASE_ADDRESS)

#define SPI_vect_0              SPI0_vect_no

//! \name spi clock-gating
//! @{
#define SIM_SCGC_SPI            SIM_SCGC6
#define SIM_SCGC_SPI0_MSK      SIM_SCGC6_SPI0_MSK
#define SIM_SCGC_SPI0_SHIFT     SIM_SCGC6_SPI0_SHIFT
//! @}


/*----------------------------------------------------------------------------*
 * ADC Device Dependent Specification                                         *
 *----------------------------------------------------------------------------*/
#define GSF_ADC_COUNT           1
#define GSF_ADC0                0
#define GSF_ADC_SC1_COUNT       2

//! \name adc power-gating
//! @{
#define SIM_SCGC_ADC            SIM_SCGC6
#define SIM_SCGC_ADC0_MSK       SIM_SCGC6_ADC0_MASK
#define SIM_SCGC_ADC0_SHIFT     SIM_SCGC6_ADC0_SHIFT
//! @}


/*----------------------------------------------------------------------------*
 * Sleep / Low power management Specification                                 *
 *----------------------------------------------------------------------------*/

/*! \note you can override this settings by redefine them at high level app_cfg.h
 */
#ifndef CORE_SLEEP_LOW_LEAKAGE_STOP
#   define CORE_SLEEP_LOW_LEAKAGE_STOP              ENABLED
#endif
#ifndef CORE_SLEEP_VERY_LOW_LEAKAGE_STOP
#   define CORE_SLEEP_VERY_LOW_LEAKAGE_STOP         ENABLED
#endif

/*----------------------------------------------------------------------------*
 * Flash Module Controller Specification                                      *
 *----------------------------------------------------------------------------*/
//! \brief flash module controller
#define FTFL        (*(flashc_t *)FTFL_BASE_PTR)

#define FLASH_SECTOR_SIZE       (1024ul)
#define PROG_FLASH_START        (0x00000000ul)
#define PROG_FLASH_END          (128ul*1024ul)
#define DATA_FLASH_START        (0x10000000ul)
#define DATA_FLASH_END          (0x10000000ul+32*1024ul)
#define FLEX_RAM_START          (0x14000000ul)
#define FLASH_CONFIG_START      (0x00000400ul)
#define FLASH_CONFIG_END        (0x00000410ul)

#define UID_BASE_ADDRESS        (0x40048054u)

#define CHIP_UID                (*((chip_uid_t *)UID_BASE_ADDRESS))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ MACROFIED EVENT HANDLER =======================*/

#define ON_ENTERING_LOW_POWER_RUN_MODE  \
        do {\
            on_normal_run_mode_event(LEAVING_NORMAL_RUN_MODE);\
            on_very_low_power_run_mode_event(ENTERING_VERY_LOW_POWER_RUN_MODE);\
        } while (false);

#define ON_CONFIRM_LOW_POWER_RUN_MODE(__RESULT) \
        do {\
            if (__RESULT) {\
                on_very_low_power_run_mode_event(VERY_LOW_POWER_RUN_MODE_ENTERED);\
            } else {\
                on_very_low_power_run_mode_event(LEAVING_VERY_LOW_POWER_RUN_MODE);\
                on_normal_run_mode_event(NORMAL_RUN_MODE_ENTERED);\
            }\
        } while (false)

#define ON_LEAVING_LOW_POWER_RUN    \
        do {\
            on_very_low_power_run_mode_event(LEAVING_VERY_LOW_POWER_RUN_MODE);\
            on_normal_run_mode_event(ENTERING_NORMAL_RUN_MODE);\
        } while (false)

#define ON_ENTER_NORMAL_RUN_MODE    \
        do {\
            on_normal_run_mode_event(NORMAL_RUN_MODE_ENTERED);\
        } while (false)


/*============================ TYPES =========================================*/
#ifndef __REG_TYPE__
#define __REG_TYPE__

typedef volatile unsigned char     reg8_t;
typedef volatile unsigned short    reg16_t;
typedef volatile unsigned int      reg32_t;

#endif


//! \name run mode event status
//! @{
typedef enum {
    ENTERING_VERY_LOW_POWER_RUN_MODE,   //!< before entering very low power run mode
    LEAVING_VERY_LOW_POWER_RUN_MODE,    //!< before leaving very low power run mode
    VERY_LOW_POWER_RUN_MODE_ENTERED,    //!< very low power run mode entered
    ENTERING_NORMAL_RUN_MODE,           //!< before entering normal run mode
    LEAVING_NORMAL_RUN_MODE,            //!< before leaving normal run mode
    NORMAL_RUN_MODE_ENTERED             //!< normal run mode entered
}run_mode_event_t;
//! @}

/*----------------------------------------------------------------------------*
 * GPIO Device Dependent Specification                                        *
 *----------------------------------------------------------------------------*/

//! \name gpio 
//! @{
typedef volatile struct {
    reg32_t OUT;                                    /**< Port Data Output Register, offset: 0x0 */
    reg32_t OUTSET;                                 /**< Port Set Output Register, offset: 0x4 */
    reg32_t OUTCLR;                                 /**< Port Clear Output Register, offset: 0x8 */
    reg32_t OUTTOG;                                 /**< Port Toggle Output Register, offset: 0xC */
    reg32_t IN;                                     /**< Port Data Input Register, offset: 0x10 */
    reg32_t DIR;                                    /**< Port Data Direction Register, offset: 0x14 */
    reg32_t         : 32;
    reg32_t         : 32;
    
    REG_RSVD_0x10
    REG_RSVD_0x10

//    uint8_t  _[0x40 - sizeof(struct GPIO_MemMap)];
}gpio_reg_t;
//! @}

////! \brief gpio array
typedef gpio_reg_t k20_gpio_array_t[GPIO_COUNT];


//! \name port
//! @{
typedef volatile struct {
    uint32_t PCR[32];                                /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */

    uint32_t GPCLR;                                  /**< Global Pin Control Low Register, offset: 0x80 */
    uint32_t GPCHR;                                  /**< Global Pin Control High Register, offset: 0x84 */
    uint32_t        : 32;
    uint32_t        : 32;

    REG_RSVD_0x10

    uint32_t ISFR;                                   /**< Interrupt Status Flag Register, offset: 0xA0 */
    uint32_t        : 32;
    uint32_t        : 32;
    uint32_t        : 32;

    REG_RSVD_0x10

    uint32_t DFER;                                   /**< Digital Filter Enable Register, offset: 0xC0 */
    uint32_t DFCR;                                   /**< Digital Filter Clock Register, offset: 0xC4 */
    uint32_t DFWR;                                   /**< Digital Filter Width Register, offset: 0xC8 */
    uint8_t _[0x1000 - sizeof(struct PORT_MemMap)];
}port_t;
//! @}

//! \name port
//! @{
typedef volatile struct {
    DEF_REG
        reg32_t PS          : 1;
        reg32_t PE          : 1;
        reg32_t             : 3;
        reg32_t ODE         : 1;
        reg32_t DS          : 1;
        reg32_t             : 1;
        reg32_t FSEL        : 3;
        reg32_t             : 21;
    END_DEF_REG(PIN[32])                            /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */

    reg32_t GPCLR;                                  /**< Global Pin Control Low Register, offset: 0x80 */
    reg32_t GPCHR;                                  /**< Global Pin Control High Register, offset: 0x84 */
    reg32_t        : 32;
    reg32_t        : 32;

    REG_RSVD_0x10

    reg32_t ISFR;                                   /**< Interrupt Status Flag Register, offset: 0xA0 */
    reg32_t        : 32;
    reg32_t        : 32;
    reg32_t        : 32;

    REG_RSVD_0x10

    reg32_t DFER;                                   /**< Digital Filter Enable Register, offset: 0xC0 */
    reg32_t DFCR;                                   /**< Digital Filter Clock Register, offset: 0xC4 */
    reg32_t DFWR;                                   /**< Digital Filter Width Register, offset: 0xC8 */
    uint8_t _[0x1000 - sizeof(struct PORT_MemMap)];
}ioctrl_reg_t;
//! @}

//! \brief gpio array
typedef port_t k20_port_array_t[GPIO_COUNT];

//! \name spi
//! @{
typedef volatile struct {
    uint32_t MCR;                                    /**< DSPI Module Configuration Register, offset: 0x0 */
    uint8_t RESERVED_0[4];
    uint32_t TCR;                                    /**< DSPI Transfer Count Register, offset: 0x8 */
    union {                                          /* offset: 0xC */
        uint32_t CTAR[2];                                /**< DSPI Clock and Transfer Attributes Register (In Master Mode), array offset: 0xC, array step: 0x4 */
        uint32_t CTAR_SLAVE[1];                          /**< DSPI Clock and Transfer Attributes Register (In Slave Mode), array offset: 0xC, array step: 0x4 */
    };
    uint8_t RESERVED_1[24];
    uint32_t SR;                                     /**< DSPI Status Register, offset: 0x2C */
    uint32_t RSER;                                   /**< DSPI DMA/Interrupt Request Select and Enable Register, offset: 0x30 */
    union {                                          /* offset: 0x34 */
        uint32_t PUSHR;                                  /**< DSPI PUSH TX FIFO Register In Master Mode, offset: 0x34 */
        uint32_t PUSHR_SLAVE;                            /**< DSPI PUSH TX FIFO Register In Slave Mode, offset: 0x34 */
    };
    uint32_t POPR;                                   /**< DSPI POP RX FIFO Register, offset: 0x38 */
    uint32_t TXFR0;                                  /**< DSPI Transmit FIFO Registers, offset: 0x3C */
    uint32_t TXFR1;                                  /**< DSPI Transmit FIFO Registers, offset: 0x40 */
    uint32_t TXFR2;                                  /**< DSPI Transmit FIFO Registers, offset: 0x44 */
    uint32_t TXFR3;                                  /**< DSPI Transmit FIFO Registers, offset: 0x48 */
    uint8_t RESERVED_2[48];
    uint32_t RXFR0;                                  /**< DSPI Receive FIFO Registers, offset: 0x7C */
    uint32_t RXFR1;                                  /**< DSPI Receive FIFO Registers, offset: 0x80 */
    uint32_t RXFR2;                                  /**< DSPI Receive FIFO Registers, offset: 0x84 */
    uint32_t RXFR3;                                  /**< DSPI Receive FIFO Registers, offset: 0x88 */

    uint8_t _[0x1000 - sizeof(struct SPI_MemMap)];
}spi_t;
//! @}

//! \brief spi array
typedef spi_t k20_spi_array_t[GSF_SPI_COUNT];


//! \name flashc
//! @{
typedef volatile struct {
    uint8_t FSTAT;                                   /**< Flash Status Register, offset: 0x0 */
    uint8_t FCNFG;                                   /**< Flash Configuration Register, offset: 0x1 */
    uint8_t FSEC;                                    /**< Flash Security Register, offset: 0x2 */
    uint8_t FOPT;                                    /**< Flash Option Register, offset: 0x3 */
    uint8_t FCCOB3;                                  /**< Flash Common Command Object Registers, offset: 0x4 */
    uint8_t FCCOB2;                                  /**< Flash Common Command Object Registers, offset: 0x5 */
    uint8_t FCCOB1;                                  /**< Flash Common Command Object Registers, offset: 0x6 */
    uint8_t FCCOB0;                                  /**< Flash Common Command Object Registers, offset: 0x7 */
    uint8_t FCCOB7;                                  /**< Flash Common Command Object Registers, offset: 0x8 */
    uint8_t FCCOB6;                                  /**< Flash Common Command Object Registers, offset: 0x9 */
    uint8_t FCCOB5;                                  /**< Flash Common Command Object Registers, offset: 0xA */
    uint8_t FCCOB4;                                  /**< Flash Common Command Object Registers, offset: 0xB */
    uint8_t FCCOBB;                                  /**< Flash Common Command Object Registers, offset: 0xC */
    uint8_t FCCOBA;                                  /**< Flash Common Command Object Registers, offset: 0xD */
    uint8_t FCCOB9;                                  /**< Flash Common Command Object Registers, offset: 0xE */
    uint8_t FCCOB8;                                  /**< Flash Common Command Object Registers, offset: 0xF */
    uint8_t FPROT3;                                  /**< Program Flash Protection Registers, offset: 0x10 */
    uint8_t FPROT2;                                  /**< Program Flash Protection Registers, offset: 0x11 */
    uint8_t FPROT1;                                  /**< Program Flash Protection Registers, offset: 0x12 */
    uint8_t FPROT0;                                  /**< Program Flash Protection Registers, offset: 0x13 */
    uint8_t RESERVED_0[2];
    uint8_t FEPROT;                                  /**< EEPROM Protection Register, offset: 0x16 */
    uint8_t FDPROT;                                  /**< Data Flash Protection Register, offset: 0x17 */
}flashc_t;
//! @}

//! \brief IO functions
//! @{
typedef enum io_func_sel_t {
    IO_WORKS_AS_FUNC0       = 0x00,
    IO_WORKS_AS_FUNC1       = 0x01,
    IO_WORKS_AS_FUNC2       = 0x02,
    IO_WORKS_AS_FUNC3       = 0x03,
    IO_WORKS_AS_FUNC4       = 0x04,
    IO_WORKS_AS_FUNC5       = 0x05,
    IO_WORKS_AS_FUNC6       = 0x06,
    IO_WORKS_AS_FUNC7       = 0x07,
}em_io_func_sel_t;
//! @}

//! \brief 16byte chip unique ID
typedef uint8_t chip_uid_t[16];

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern void on_very_low_power_run_mode_event(run_mode_event_t tStatus);
extern void on_normal_run_mode_event(run_mode_event_t tStatus);


#endif