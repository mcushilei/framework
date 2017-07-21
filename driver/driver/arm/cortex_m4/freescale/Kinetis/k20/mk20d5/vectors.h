/******************************************************************************
* File:    vectors.h
*
* Purpose: Provide custom interrupt service routines for Kinetis. 
*
* NOTE: This vector table is a superset table, so interrupt sources might be 
*       listed that are not available on the specific Kinetis device you are 
*       using.
******************************************************************************/

#ifndef __VECTORS_H__
#define __VECTORS_H__     


/*============================ MACROS ========================================*/

#define VECTOR_COUNT        (62)

//! \name vector number
//! @{
#define  DMA0_vect_no        0         //16 
#define  DMA1_vect_no        1         //17 
#define  DMA2_vect_no        2         //18 
#define  DMA3_vect_no        3         //19 
#define  DMA4_vect_no        4         //20 
#define  DMA5_vect_no        5         //21 
#define  FLASH_CC_vect_no    6         //22 
#define  FLASH_RC_vect_no    7         //23 
#define  LVD_vect_no         8         //24 
#define  LLWU_vect_no        9         //25 
#define  WDOG_vect_no        10        //26 
#define  I2C0_vect_no        11        //27 
#define  SPI0_vect_no        12        //28 
#define  I2S0_vect_no        13        //29 
#define  I2S1_vect_no        14        //30 
#define  UART0_LON_vect_no   15        //31 
#define  UART0_vect_no       16        //32 
#define  UART0_ERR_vect_no   17        //33 
#define  UART1_vect_no       18        //34 
#define  UART1_ERR_vect_no   19        //35 
#define  UART2_vect_no       20        //36 
#define  UART2_ERR_vect_no   21        //37 
#define  ADC0_vect_no        22        //38 
#define  CMP0_vect_no      	 23        //39
#define  CMP1_vect_no        24        //40 
#define  FTM0_vect_no        25        //41 
#define  FTM1_vect_no        26        //42 
#define  CMT_vect_no         27        //43 
#define  RTCA_vect_no        28        //44 
#define  RTCS_vect_no        29        //45 
#define  PITC0_vect_no       30        //46 
#define  PITC1_vect_no       31        //47 
#define  PITC2_vect_no       32        //48 
#define  PITC3_vect_no       33        //49 
#define  PDB_vect_no         34        //50 
#define  USB_OTG_vect_no     35        //51 
#define  USB_CD_vect_no      36        //52 
#define  TSI_vect_no 	     37        //53 
#define  MCG_vect_no         38        //54 
#define  LPT_vect_no         39        //55 
#define  PORTA_vect_no       40        //56 
#define  PORTB_vect_no       41        //57 
#define  PORTC_vect_no       42        //58 
#define  PORTD_vect_no       43        //59 
#define  PORTE_vect_no       44        //60 
#define  SWI_vect_no         45        //61
//! @}

////! \name vector table
////! @{
//#ifdef vect_DMA0
//#define  VECTOR_016         vect_DMA0       //16 
//#endif
//#ifdef vect_DMA1
//#define  VECTOR_017         vect_DMA1       //17 
//#endif
//#ifdef vect_DMA2
//#define  VECTOR_018         vect_DMA2       //18 
//#endif
//#ifdef vect_DMA3
//#define  VECTOR_019         vect_DMA3       //19 
//#endif
//#ifdef vect_DMA4
//#define  VECTOR_020         vect_DMA4       //20 
//#endif
//#ifdef vect_DMA5
//#define  VECTOR_021         vect_DMA5       //21 
//#endif
//#ifdef vect_FCmdCom
//#define  VECTOR_022         vect_FCmdCom    //22 
//#endif
//#ifdef vect_FReadCol
//#define  VECTOR_023         vect_FReadCol   //23 
//#endif
//#ifdef vect_LVD
//#define  VECTOR_024         vect_LVD        //24 
//#endif
//#ifdef vect_LLWU
//#define  VECTOR_025         vect_LLWU       //25 
//#endif
//#ifdef vect_WDOG
//#define  VECTOR_026         vect_WDOG       //26 
//#endif
//#ifdef vect_I2C0
//#define  VECTOR_027         vect_I2C0       //27 
//#endif
//#ifdef vect_SPI0
//#define  VECTOR_028         vect_SPI0       //28
//#endif
//#ifdef vect_I2S0
//#define  VECTOR_029         vect_I2S0       //29 
//#endif
//#ifdef vect_I2S1
//#define  VECTOR_030         vect_I2S1       //30 
//#endif
//#ifdef vect_UART0LON
//#define  VECTOR_031         vect_UART0LON   //31 
//#endif
//#ifdef vect_UART0S
//#define  VECTOR_032         vect_UART0S     //32 
//#endif
//#ifdef vect_UART0E
//#define  VECTOR_033         vect_UART0E     //33 
//#endif
//#ifdef vect_UART1S
//#define  VECTOR_034         vect_UART1S     //34 
//#endif
//#ifdef vect_UART1E
//#define  VECTOR_035         vect_UART1E     //35 
//#endif
//#ifdef vect_UART2S
//#define  VECTOR_036         vect_UART2S     //36
//#endif
//#ifdef vect_UART2E
//#define  VECTOR_037         vect_UART2E     //37 
//#endif
//#ifdef vect_ADC0
//#define  VECTOR_038         vect_ADC0       //38
//#endif
//#ifdef vect_CMP0
//#define  VECTOR_039         vect_CMP0       //39
//#endif
//#ifdef vect_CMP1
//#define  VECTOR_040         vect_CMP1       //40 
//#endif
//#ifdef vect_FTM0
//#define  VECTOR_041         vect_FTM0       //41 
//#endif
//#ifdef vect_FTM1
//#define  VECTOR_042         vect_FTM1       //42 
//#endif
//#ifdef vect_CMT
//#define  VECTOR_043         vect_CMT        //43 
//#endif
//#ifdef vect_RTCA
//#define  VECTOR_044         vect_RTCA       //44
//#endif
//#ifdef vect_RTCS
//#define  VECTOR_045         vect_RTCS       //45 
//#endif
//#ifdef vect_PITC0
//#define  VECTOR_046         vect_PITC0      //46 
//#endif
//#ifdef vect_PITC1
//#define  VECTOR_047         vect_PITC1      //47 
//#endif
//#ifdef vect_PITC2
//#define  VECTOR_048         vect_PITC2      //48 
//#endif
//#ifdef vect_PITC3
//#define  VECTOR_049         vect_PITC3      //49 
//#endif
//#ifdef vect_PDB
//#define  VECTOR_050         vect_PDB        //50
//#endif
//#ifdef vect_USBOTG
//#define  VECTOR_051         vect_USBOTG     //51 
//#endif
//#ifdef vect_USBCD
//#define  VECTOR_052         vect_USBCD      //52 
//#endif
//#ifdef vect_TSI
//#define  VECTOR_053         vect_TSI        //53 
//#endif
//#ifdef vect_MCG
//#define  VECTOR_054         vect_MCG        //54 
//#endif
//#ifdef vect_LPTMR
//#define  VECTOR_055         vect_LPTMR      //55 
//#endif
//#ifdef vect_PortA
//#define  VECTOR_056         vect_PortA      //56 
//#endif
//#ifdef vect_PortB
//#define  VECTOR_057         vect_PortB      //57
//#endif
//#ifdef vect_PortC
//#define  VECTOR_058         vect_PortC      //58 
//#endif
//#ifdef vect_PortD
//#define  VECTOR_059         vect_PortD      //59 
//#endif
//#ifdef vect_PortE
//#define  VECTOR_060         vect_PortE      //60 
//#endif
//#ifdef vect_SWI
//#define  VECTOR_061         vect_SWI        //61
//#endif
////! @}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif /*__VECTORS_H*/

/* End of "vectors.h" */
