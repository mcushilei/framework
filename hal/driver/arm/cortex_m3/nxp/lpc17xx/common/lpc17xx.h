/**************************************************************************//**
 * @file     LPC17xx.h
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Header File for
 *           NXP LPC17xx Device Series
 * @version: V1.09
 * @date:    25. July. 2011
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __LPC17xx_H__
#define __LPC17xx_H__


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

/*------------- Timer (TIM) --------------------------------------------------*/
/** @brief Timer (TIM) register structure definition */
typedef struct
{
      reg32_t IR;
      reg32_t TCR;
      reg32_t TC;
      reg32_t PR;
      reg32_t PC;
      reg32_t MCR;
      reg32_t MR0;
      reg32_t MR1;
      reg32_t MR2;
      reg32_t MR3;
      reg32_t CCR;
      reg32_t CR0;
      reg32_t CR1;
      reg32_t RESERVED0[2];
      reg32_t EMR;
      reg32_t RESERVED1[12];
      reg32_t CTCR;
} tim_reg_t;

/*------------- Pulse-Width Modulation (PWM) ---------------------------------*/
/** @brief Pulse-Width Modulation (PWM) register structure definition */
typedef struct
{
      reg32_t IR;
      reg32_t TCR;
      reg32_t TC;
      reg32_t PR;
      reg32_t PC;
      reg32_t MCR;
      reg32_t MR0;
      reg32_t MR1;
      reg32_t MR2;
      reg32_t MR3;
      reg32_t CCR;
      reg32_t CR0;
      reg32_t CR1;
      reg32_t CR2;
      reg32_t CR3;
      reg32_t RESERVED0;
      reg32_t MR4;
      reg32_t MR5;
      reg32_t MR6;
      reg32_t PCR;
      reg32_t LER;
      reg32_t RESERVED1[7];
      reg32_t CTCR;
} pwm_reg_t;


/*------------- Inter IC Sound (I2S) -----------------------------------------*/
/** @brief  Inter IC Sound (I2S) register structure definition */
typedef struct
{
      reg32_t I2SDAO;
      reg32_t I2SDAI;
      reg32_t I2STXFIFO;
      reg32_t I2SRXFIFO;
      reg32_t I2SSTATE;
      reg32_t I2SDMA1;
      reg32_t I2SDMA2;
      reg32_t I2SIRQ;
      reg32_t I2STXRATE;
      reg32_t I2SRXRATE;
      reg32_t I2STXBITRATE;
      reg32_t I2SRXBITRATE;
      reg32_t I2STXMODE;
      reg32_t I2SRXMODE;
} i2s_reg_t;

/*------------- Repetitive Interrupt Timer (RIT) -----------------------------*/
/** @brief  Repetitive Interrupt Timer (RIT) register structure definition */
typedef struct
{
      reg32_t RICOMPVAL;
      reg32_t RIMASK;
    reg8_t  RICTRL;
       uint8_t  RESERVED0[3];
      reg32_t RICOUNTER;
} rit_reg_t;


/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
/** @brief  Analog-to-Digital Converter (ADC) register structure definition */
typedef struct
{
      reg32_t ADCR;
      reg32_t ADGDR;
      reg32_t RESERVED0;
      reg32_t ADINTEN;
      reg32_t ADDR0;
      reg32_t ADDR1;
      reg32_t ADDR2;
      reg32_t ADDR3;
      reg32_t ADDR4;
      reg32_t ADDR5;
      reg32_t ADDR6;
      reg32_t ADDR7;
      reg32_t ADSTAT;
      reg32_t ADTRM;
} adc_reg_t;

/*------------- Digital-to-Analog Converter (DAC) ----------------------------*/
/** @brief  Digital-to-Analog Converter (DAC) register structure definition */
typedef struct
{
      reg32_t DACR;
      reg32_t DACCTRL;
      reg16_t DACCNTVAL;
} dac_reg_t;

/*------------- Motor Control Pulse-Width Modulation (MCPWM) -----------------*/
/** @brief  Motor Control Pulse-Width Modulation (MCPWM) register structure definition */
typedef struct
{
      reg32_t MCCON;
      reg32_t MCCON_SET;
      reg32_t MCCON_CLR;
      reg32_t MCCAPCON;
      reg32_t MCCAPCON_SET;
      reg32_t MCCAPCON_CLR;
      reg32_t MCTIM0;
      reg32_t MCTIM1;
      reg32_t MCTIM2;
      reg32_t MCPER0;
      reg32_t MCPER1;
      reg32_t MCPER2;
      reg32_t MCPW0;
      reg32_t MCPW1;
      reg32_t MCPW2;
      reg32_t MCDEADTIME;
      reg32_t MCCCP;
      reg32_t MCCR0;
      reg32_t MCCR1;
      reg32_t MCCR2;
      reg32_t MCINTEN;
      reg32_t MCINTEN_SET;
      reg32_t MCINTEN_CLR;
      reg32_t MCCNTCON;
      reg32_t MCCNTCON_SET;
      reg32_t MCCNTCON_CLR;
      reg32_t MCINTFLAG;
      reg32_t MCINTFLAG_SET;
      reg32_t MCINTFLAG_CLR;
      reg32_t MCCAP_CLR;
} wcpwm_reg_t;

/*------------- Quadrature Encoder Interface (QEI) ---------------------------*/
/** @brief  Quadrature Encoder Interface (QEI) register structure definition */
typedef struct
{
      reg32_t QEICON;
      reg32_t QEISTAT;
      reg32_t QEICONF;
      reg32_t QEIPOS;
      reg32_t QEIMAXPOS;
      reg32_t CMPOS0;
      reg32_t CMPOS1;
      reg32_t CMPOS2;
      reg32_t INXCNT;
      reg32_t INXCMP;
      reg32_t QEILOAD;
      reg32_t QEITIME;
      reg32_t QEIVEL;
      reg32_t QEICAP;
      reg32_t VELCOMP;
      reg32_t FILTER;
      reg32_t RESERVED0[998];
      reg32_t QEIIEC;
      reg32_t QEIIES;
      reg32_t QEIINTSTAT;
      reg32_t QEIIE;
      reg32_t QEICLR;
      reg32_t QEISET;
} qei_reg_t;

/*------------- Controller Area Network (CAN) --------------------------------*/
/** @brief  Controller Area Network Acceptance Filter RAM (CANAF_RAM)structure definition */
typedef struct
{
      reg32_t mask[512];              /* ID Masks                           */
} canaf_ram_reg_t;

/** @brief  Controller Area Network Acceptance Filter(CANAF) register structure definition */
typedef struct                          /* Acceptance Filter Registers        */
{
      reg32_t AFMR;
      reg32_t SFF_sa;
      reg32_t SFF_GRP_sa;
      reg32_t EFF_sa;
      reg32_t EFF_GRP_sa;
      reg32_t ENDofTable;
      reg32_t LUTerrAd;
      reg32_t LUTerr;
      reg32_t FCANIE;
      reg32_t FCANIC0;
      reg32_t FCANIC1;
} canaf_reg_t;

/** @brief  Controller Area Network Central (CANCR) register structure definition */
typedef struct                          /* Central Registers                  */
{
      reg32_t CANTxSR;
      reg32_t CANRxSR;
      reg32_t CANMSR;
} cancr_reg_t;

/** @brief  Controller Area Network Controller (CAN) register structure definition */
typedef struct                          /* Controller Registers               */
{
      reg32_t MOD;
      reg32_t CMR;
      reg32_t GSR;
      reg32_t ICR;
      reg32_t IER;
      reg32_t BTR;
      reg32_t EWL;
      reg32_t SR;
      reg32_t RFS;
      reg32_t RID;
      reg32_t RDA;
      reg32_t RDB;
      reg32_t TFI1;
      reg32_t TID1;
      reg32_t TDA1;
      reg32_t TDB1;
      reg32_t TFI2;
      reg32_t TID2;
      reg32_t TDA2;
      reg32_t TDB2;
      reg32_t TFI3;
      reg32_t TID3;
      reg32_t TDA3;
      reg32_t TDB3;
} can_reg_t;

/*------------- General Purpose Direct Memory Access (GPDMA) -----------------*/
/** @brief  General Purpose Direct Memory Access (GPDMA) register structure definition */
typedef struct                          /* Common Registers                   */
{
      reg32_t DMACIntStat;
      reg32_t DMACIntTCStat;
      reg32_t DMACIntTCClear;
      reg32_t DMACIntErrStat;
      reg32_t DMACIntErrClr;
      reg32_t DMACRawIntTCStat;
      reg32_t DMACRawIntErrStat;
      reg32_t DMACEnbldChns;
      reg32_t DMACSoftBReq;
      reg32_t DMACSoftSReq;
      reg32_t DMACSoftLBReq;
      reg32_t DMACSoftLSReq;
      reg32_t DMACConfig;
      reg32_t DMACSync;
} gpdma_reg_t;

/** @brief  General Purpose Direct Memory Access Channel (GPDMACH) register structure definition */
typedef struct                          /* Channel Registers                  */
{
      reg32_t DMACCSrcAddr;
      reg32_t DMACCDestAddr;
      reg32_t DMACCLLI;
      reg32_t DMACCControl;
      reg32_t DMACCConfig;
} gpdmach_reg_t;

/*------------- Universal Serial Bus (USB) -----------------------------------*/
/** @brief  Universal Serial Bus (USB) register structure definition */
typedef struct
{
      reg32_t HcRevision;             /* USB Host Registers                 */
      reg32_t HcControl;
      reg32_t HcCommandStatus;
      reg32_t HcInterruptStatus;
      reg32_t HcInterruptEnable;
      reg32_t HcInterruptDisable;
      reg32_t HcHCCA;
      reg32_t HcPeriodCurrentED;
      reg32_t HcControlHeadED;
      reg32_t HcControlCurrentED;
      reg32_t HcBulkHeadED;
      reg32_t HcBulkCurrentED;
      reg32_t HcDoneHead;
      reg32_t HcFmInterval;
      reg32_t HcFmRemaining;
      reg32_t HcFmNumber;
      reg32_t HcPeriodicStart;
      reg32_t HcLSTreshold;
      reg32_t HcRhDescriptorA;
      reg32_t HcRhDescriptorB;
      reg32_t HcRhStatus;
      reg32_t HcRhPortStatus1;
      reg32_t HcRhPortStatus2;
      reg32_t RESERVED0[40];
      reg32_t Module_ID;

      reg32_t OTGIntSt;               /* USB On-The-Go Registers            */
      reg32_t OTGIntEn;
      reg32_t OTGIntSet;
      reg32_t OTGIntClr;
      reg32_t OTGStCtrl;
      reg32_t OTGTmr;
      reg32_t RESERVED1[58];

      reg32_t USBDevIntSt;            /* USB Device Interrupt Registers     */
      reg32_t USBDevIntEn;
      reg32_t USBDevIntClr;
      reg32_t USBDevIntSet;

      reg32_t USBCmdCode;             /* USB Device SIE Command Registers   */
      reg32_t USBCmdData;

      reg32_t USBRxData;              /* USB Device Transfer Registers      */
      reg32_t USBTxData;
      reg32_t USBRxPLen;
      reg32_t USBTxPLen;
      reg32_t USBCtrl;
      reg32_t USBDevIntPri;

      reg32_t USBEpIntSt;             /* USB Device Endpoint Interrupt Regs */
      reg32_t USBEpIntEn;
      reg32_t USBEpIntClr;
      reg32_t USBEpIntSet;
      reg32_t USBEpIntPri;

      reg32_t USBReEp;                /* USB Device Endpoint Realization Reg*/
      reg32_t USBEpInd;
      reg32_t USBMaxPSize;

      reg32_t USBDMARSt;              /* USB Device DMA Registers           */
      reg32_t USBDMARClr;
      reg32_t USBDMARSet;
      reg32_t RESERVED2[9];
      reg32_t USBUDCAH;
      reg32_t USBEpDMASt;
      reg32_t USBEpDMAEn;
      reg32_t USBEpDMADis;
      reg32_t USBDMAIntSt;
      reg32_t USBDMAIntEn;
      reg32_t RESERVED3[2];
      reg32_t USBEoTIntSt;
      reg32_t USBEoTIntClr;
      reg32_t USBEoTIntSet;
      reg32_t USBNDDRIntSt;
      reg32_t USBNDDRIntClr;
      reg32_t USBNDDRIntSet;
      reg32_t USBSysErrIntSt;
      reg32_t USBSysErrIntClr;
      reg32_t USBSysErrIntSet;
      reg32_t RESERVED4[15];

  union {
      reg32_t I2C_RX;                 /* USB OTG I2C Registers              */
      reg32_t I2C_TX;
  };
      reg32_t I2C_STS;
      reg32_t I2C_CTL;
      reg32_t I2C_CLKHI;
      reg32_t I2C_CLKLO;
      reg32_t RESERVED5[824];

  union {
      reg32_t USBClkCtrl;             /* USB Clock Control Registers        */
      reg32_t OTGClkCtrl;
  };
  union {
      reg32_t USBClkSt;
      reg32_t OTGClkSt;
  };
} usb_reg_t;

/*------------- Ethernet Media Access Controller (EMAC) ----------------------*/
/** @brief  Ethernet Media Access Controller (EMAC) register structure definition */
typedef struct
{
      reg32_t MAC1;                   /* MAC Registers                      */
      reg32_t MAC2;
      reg32_t IPGT;
      reg32_t IPGR;
      reg32_t CLRT;
      reg32_t MAXF;
      reg32_t SUPP;
      reg32_t TEST;
      reg32_t MCFG;
      reg32_t MCMD;
      reg32_t MADR;
      reg32_t MWTD;
      reg32_t MRDD;
      reg32_t MIND;
      reg32_t RESERVED0[2];
      reg32_t SA0;
      reg32_t SA1;
      reg32_t SA2;
      reg32_t RESERVED1[45];
      reg32_t Command;                /* Control Registers                  */
      reg32_t Status;
      reg32_t RxDescriptor;
      reg32_t RxStatus;
      reg32_t RxDescriptorNumber;
      reg32_t RxProduceIndex;
      reg32_t RxConsumeIndex;
      reg32_t TxDescriptor;
      reg32_t TxStatus;
      reg32_t TxDescriptorNumber;
      reg32_t TxProduceIndex;
      reg32_t TxConsumeIndex;
      reg32_t RESERVED2[10];
      reg32_t TSV0;
      reg32_t TSV1;
      reg32_t RSV;
      reg32_t RESERVED3[3];
      reg32_t FlowControlCounter;
      reg32_t FlowControlStatus;
      reg32_t RESERVED4[34];
      reg32_t RxFilterCtrl;           /* Rx Filter Registers                */
      reg32_t RxFilterWoLStatus;
      reg32_t RxFilterWoLClear;
      reg32_t RESERVED5;
      reg32_t HashFilterL;
      reg32_t HashFilterH;
      reg32_t RESERVED6[882];
      reg32_t IntStatus;              /* Module Control Registers           */
      reg32_t IntEnable;
      reg32_t IntClear;
      reg32_t IntSet;
      reg32_t RESERVED7;
      reg32_t PowerDown;
      reg32_t RESERVED8;
      reg32_t Module_ID;
} emac_reg_t;

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define FLASH_BASE          (0x00000000UL)
#define RAM_BASE            (0x10000000UL)
#ifdef __LPC17XX_REV00
#define AHBRAM0_BASE        (0x20000000UL)
#define AHBRAM1_BASE        (0x20004000UL)
#else
#define AHBRAM0_BASE        (0x2007C000UL)
#define AHBRAM1_BASE        (0x20080000UL)
#endif
#define GPIO_BASE           (0x2009C000UL)
#define APB0_BASE           (0x40000000UL)
#define APB1_BASE           (0x40080000UL)
#define AHB_BASE            (0x50000000UL)
#define CM3_BASE            (0xE0000000UL)

/* APB0 peripherals                                                           */
#define WDT_BASE            (APB0_BASE + 0x00000)
#define TIM0_BASE           (APB0_BASE + 0x04000)
#define TIM1_BASE           (APB0_BASE + 0x08000)
#define UART0_BASE          (APB0_BASE + 0x0C000)
#define UART1_BASE          (APB0_BASE + 0x10000)
#define PWM1_BASE           (APB0_BASE + 0x18000)
#define I2C0_BASE           (APB0_BASE + 0x1C000)
#define SPI_BASE            (APB0_BASE + 0x20000)
#define RTC_BASE            (APB0_BASE + 0x24000)
#define GPIOINT_BASE        (APB0_BASE + 0x28080)
#define PINCON_BASE         (APB0_BASE + 0x2C000)
#define SSP1_BASE           (APB0_BASE + 0x30000)
#define ADC_BASE            (APB0_BASE + 0x34000)
#define CANAF_RAM_BASE      (APB0_BASE + 0x38000)
#define CANAF_BASE          (APB0_BASE + 0x3C000)
#define CANCR_BASE          (APB0_BASE + 0x40000)
#define CAN1_BASE           (APB0_BASE + 0x44000)
#define CAN2_BASE           (APB0_BASE + 0x48000)
#define I2C1_BASE           (APB0_BASE + 0x5C000)

/* APB1 peripherals                                                           */
#define SSP0_BASE           (APB1_BASE + 0x08000)
#define DAC_BASE            (APB1_BASE + 0x0C000)
#define TIM2_BASE           (APB1_BASE + 0x10000)
#define TIM3_BASE           (APB1_BASE + 0x14000)
#define UART2_BASE          (APB1_BASE + 0x18000)
#define UART3_BASE          (APB1_BASE + 0x1C000)
#define I2C2_BASE           (APB1_BASE + 0x20000)
#define I2S_BASE            (APB1_BASE + 0x28000)
#define RIT_BASE            (APB1_BASE + 0x30000)
#define MCPWM_BASE          (APB1_BASE + 0x38000)
#define QEI_BASE            (APB1_BASE + 0x3C000)
#define SC_BASE             (APB1_BASE + 0x7C000)

/* AHB peripherals                                                            */
#define EMAC_BASE           (AHB_BASE  + 0x00000)
#define GPDMA_BASE          (AHB_BASE  + 0x04000)
#define GPDMACH0_BASE       (AHB_BASE  + 0x04100)
#define GPDMACH1_BASE       (AHB_BASE  + 0x04120)
#define GPDMACH2_BASE       (AHB_BASE  + 0x04140)
#define GPDMACH3_BASE       (AHB_BASE  + 0x04160)
#define GPDMACH4_BASE       (AHB_BASE  + 0x04180)
#define GPDMACH5_BASE       (AHB_BASE  + 0x041A0)
#define GPDMACH6_BASE       (AHB_BASE  + 0x041C0)
#define GPDMACH7_BASE       (AHB_BASE  + 0x041E0)
#define USB_BASE            (AHB_BASE  + 0x0C000)

/* GPIOs                                                                      */
#define GPIO0_BASE          (GPIO_BASE + 0x00000)
#define GPIO1_BASE          (GPIO_BASE + 0x00020)
#define GPIO2_BASE          (GPIO_BASE + 0x00040)
#define GPIO3_BASE          (GPIO_BASE + 0x00060)
#define GPIO4_BASE          (GPIO_BASE + 0x00080)

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define TIM0_REG                (*(tim_reg_t       *) TIM0_BASE     )
#define TIM1_REG                (*(tim_reg_t       *) TIM1_BASE     )
#define TIM2_REG                (*(tim_reg_t       *) TIM2_BASE     )
#define TIM3_REG                (*(tim_reg_t       *) TIM3_BASE     )
#define RIT_REG                 (*(rit_reg_t       *) RIT_BASE      )
#define PWM1_REG                (*(pwm_reg_t       *) PWM1_BASE     )
#define I2S_REG                 (*(i2s_reg_t       *) I2S_BASE      )
#define ADC_REG                 (*(adc_reg_t       *) ADC_BASE      )
#define DAC_REG                 (*(dac_reg_t       *) DAC_BASE      )
#define CANAF_RAM_REG           (*(canaf_ram_reg_t *) CANAF_RAM_BASE)
#define CANAF_REG               (*(canaf_reg_t     *) CANAF_BASE    )
#define CANCR_REG               (*(cancr_reg_t     *) CANCR_BASE    )
#define CAN1_REG                (*(can_reg_t       *) CAN1_BASE     )
#define CAN2_REG                (*(can_reg_t       *) CAN2_BASE     )
#define MCPWM_REG               (*(wcpwm_reg_t     *) MCPWM_BASE    )
#define QEI_REG                 (*(qei_reg_t       *) QEI_BASE      )
#define EMAC_REG                (*(emac_reg_t      *) EMAC_BASE     )
#define GPDMA_REG               (*(gpdma_reg_t     *) GPDMA_BASE    )
#define GPDMACH0_REG            (*(gpdmach_reg_t   *) GPDMACH0_BASE )
#define GPDMACH1_REG            (*(gpdmach_reg_t   *) GPDMACH1_BASE )
#define GPDMACH2_REG            (*(gpdmach_reg_t   *) GPDMACH2_BASE )
#define GPDMACH3_REG            (*(gpdmach_reg_t   *) GPDMACH3_BASE )
#define GPDMACH4_REG            (*(gpdmach_reg_t   *) GPDMACH4_BASE )
#define GPDMACH5_REG            (*(gpdmach_reg_t   *) GPDMACH5_BASE )
#define GPDMACH6_REG            (*(gpdmach_reg_t   *) GPDMACH6_BASE )
#define GPDMACH7_REG            (*(gpdmach_reg_t   *) GPDMACH7_BASE )
#define USB_REG                 (*(usb_reg_t       *) USB_BASE      )


#define DMAReqSel           (*(reg32_t *)0x4000C1C4ul)

/**
 * @}
 */

#endif  // __LPC17xx_H__
