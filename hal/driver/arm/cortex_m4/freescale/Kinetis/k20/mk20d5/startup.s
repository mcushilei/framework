/*
 * File:	crt0.s
 * Purpose:	Lowest level routines for Kinetis.
 *
 * Notes:	
 *
 */

;;------------------------------------------------------------------------------
;; Startup
;;------------------------------------------------------------------------------
;AREA   Crt0, CODE, READONLY    ; name this block of code
    SECTION .text_bl_core  : CODE
    EXTERN  __iar_program_start
    EXPORT  __startup
__startup
        CPSID   i                   ; mask interrupts
        BL      __iar_program_start ; call the C code
__done
        B       __done

;;------------------------------------------------------------------------------
;; Vector Table
;;------------------------------------------------------------------------------

        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec:CODE:NOROOT(2)
        PUBLIC  __vector_table
        DATA
__vector_table
        DCD     sfe(CSTACK)                 ; 0  Top of Stack
        DCD     __startup                   ; 1  Reset Handler
        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler
        ; External Interrupts
        DCD     DMA0_vect           ; 0:  DMA Channel 0 transfer complete
        DCD     DMA1_vect           ; 1:  DMA Channel 1 transfer complete
        DCD     DMA2_vect           ; 2:  DMA Channel 2 transfer complete
        DCD     DMA3_vect           ; 3:  DMA Channel 3 transfer complete
        DCD     DMA4_vect           ; 4:  DMA Channel 4 transfer complete
        DCD     DMA5_vect           ; 5:  DMA Channel 5 transfer complete
        DCD     FLASH_CC_vect       ; 6:  FTFL interrupt
        DCD     FLASH_RC_vect       ; 7:  Read collision interrupt
        DCD     VLD_vect            ; 8:  Low Voltage Detect, Low Voltage Warning
        DCD     LLWU_vect           ; 9:  Low Leakage Wakeup
        DCD     WDOG_vect           ;10:  WDOG interrupt
        DCD     I2C0_vect           ;11:  I2C0 interrupt
        DCD     SPI0_vect           ;12:  SPI 0 interrupt
        DCD     I2STX_vect            ;13:  I2S0 tx interrupt
        DCD     I2SRX_vect            ;14:  I2S0 rx interrupt
        DCD     USART0_LON_vect      ;15:  UART 0 LON
        DCD     USART0_vect          ;16:  UART 0 interrupt
        DCD     USART0_ERR_vect      ;17:  UART 0 error intertrupt
        DCD     USART1_vect          ;18:  UART 1 interrupt
        DCD     USART1_ERR_vect      ;19:  UART 1 error intertrupt
        DCD     USART2_vect          ;20:  UART 2 interrupt
        DCD     USART2_ERR_vect      ;21:  UART 2 error intertrupt
        DCD     ADC0_vect           ;22:  ADC 0 interrupt
        DCD     CMP0_vect           ;23:  CMP 0 High-speed comparator interrupt
        DCD     CMP1_vect           ;24:  CMP 1 interrupt
        DCD     FTM0_vect           ;25:  FTM 0 interrupt
        DCD     FTM1_vect           ;26:  FTM 1 interrupt
        DCD     CMT_vect            ;27:  CMT intrrupt
        DCD     RTC_vect            ;28:  RTC interrupt
        DCD     RTCSEC_vect         ;29:  RTC seconds interrupt
        DCD     PIT0_vect           ;30:  PIT 0 interrupt
        DCD     PIT1_vect           ;31:  PIT 1 interrupt
        DCD     PIT2_vect           ;32:  PIT 2 interrupt
        DCD     PIT3_vect           ;33:  PIT 3 interrupt
        DCD     PDB_vect            ;34:  PDB interrupt
        DCD     USB_OTG_vect        ;35:  USB OTG interrupt
        DCD     USB_CD_vect         ;36:  USB Charger Detect interrupt
        DCD     TSI_vect            ;37:  TSI interrupt
        DCD     MCG_vect            ;38:  MCG interrupt
        DCD     LPT_vect            ;39:  LPT interrupt
        DCD     PORTA_vect          ;40:  PORT A interrupt
        DCD     PORTB_vect          ;41:  PORT B interrupt
        DCD     PORTC_vect          ;42:  PORT C interrupt
        DCD     PORTD_vect          ;43:  PORT D interrupt
        DCD     PORTE_vect          ;44:  PORT E interrupt
        DCD     SWI_vect            ;45:  Software interrupt
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;------------------------------------------------------------------------------
;; Chip-secruty configuration
;;------------------------------------------------------------------------------
;; Flash configuration field values below
;; Please be careful when modifying any of
;; the values below as it can secure the 
;; flash (possibly permanently): 0x400-0x409.

#ifndef CONFIG_1
#define CONFIG_1	0xffffffff 
#endif
#ifndef CONFIG_2
#define CONFIG_2	0xffffffff 
#endif

#ifndef CONFIG_3
#define CONFIG_3	0xffffffff 
#endif
#ifndef CONFIG_4
#define CONFIG_4	0xfffffffe 
#endif

    SECTION FlashConfig:CODE:ROOT(4)
    DATA
        DCD     CONFIG_1
        DCD     CONFIG_2
        DCD     CONFIG_3
        DCD     CONFIG_4

;;------------------------------------------------------------------------------
;; Default interrupt handlers.
;;------------------------------------------------------------------------------
    THUMB
      PUBWEAK NMI_Handler         
      PUBWEAK HardFault_Handler   
      PUBWEAK MemManage_Handler   
      PUBWEAK BusFault_Handler    
      PUBWEAK UsageFault_Handler                    
      PUBWEAK SVC_Handler         
      PUBWEAK DebugMon_Handler                  
      PUBWEAK PendSV_Handler      
      PUBWEAK SysTick_Handler

      PUBWEAK DMA0_vect     
      PUBWEAK DMA1_vect     
      PUBWEAK DMA2_vect     
      PUBWEAK DMA3_vect     
      PUBWEAK DMA4_vect  
      PUBWEAK DMA5_vect
      PUBWEAK FLASH_CC_vect 
      PUBWEAK FLASH_RC_vect 
      PUBWEAK VLD_vect      
      PUBWEAK LLWU_vect     
      PUBWEAK WDOG_vect     
      PUBWEAK I2C0_vect     
      PUBWEAK SPI0_vect     
      PUBWEAK I2STX_vect      
      PUBWEAK I2SRX_vect      
      PUBWEAK USART0_LON_vect  
      PUBWEAK USART0_vect    
      PUBWEAK USART0_ERR_vect
      PUBWEAK USART1_vect    
      PUBWEAK USART1_ERR_vect
      PUBWEAK USART2_vect    
      PUBWEAK USART2_ERR_vect
      PUBWEAK ADC0_vect     
      PUBWEAK CMP0_vect     
      PUBWEAK CMP1_vect     
      PUBWEAK FTM0_vect     
      PUBWEAK FTM1_vect     
      PUBWEAK CMT_vect      
      PUBWEAK RTC_vect      
      PUBWEAK RTCSEC_vect
      PUBWEAK PIT0_vect     
      PUBWEAK PIT1_vect     
      PUBWEAK PIT2_vect     
      PUBWEAK PIT3_vect     
      PUBWEAK PDB_vect  
      PUBWEAK USB_OTG_vect
      PUBWEAK USB_CD_vect   
      PUBWEAK TSI_vect      
      PUBWEAK MCG_vect      
      PUBWEAK LPT_vect      
      PUBWEAK PORTA_vect    
      PUBWEAK PORTB_vect    
      PUBWEAK PORTC_vect    
      PUBWEAK PORTD_vect    
      PUBWEAK PORTE_vect    
      PUBWEAK SWI_vect

      SECTION .text:CODE:REORDER(1)
      
NMI_Handler         
HardFault_Handler         
MemManage_Handler         
BusFault_Handler          
UsageFault_Handler        
SVC_Handler               
DebugMon_Handler          
PendSV_Handler            
SysTick_Handler      
DMA0_vect           
DMA1_vect           
DMA2_vect           
DMA3_vect           
DMA4_vect    
DMA5_vect
FLASH_CC_vect       
FLASH_RC_vect       
VLD_vect            
LLWU_vect           
WDOG_vect           
I2C0_vect           
SPI0_vect           
I2STX_vect          
I2SRX_vect          
USART0_LON_vect      
USART0_vect          
USART0_ERR_vect      
USART1_vect          
USART1_ERR_vect      
USART2_vect          
USART2_ERR_vect      
ADC0_vect           
CMP0_vect           
CMP1_vect           
FTM0_vect           
FTM1_vect           
CMT_vect            
RTC_vect            
RTCSEC_vect      
PIT0_vect           
PIT1_vect           
PIT2_vect           
PIT3_vect           
PDB_vect   
USB_OTG_vect             
USB_CD_vect         
TSI_vect            
MCG_vect            
LPT_vect            
PORTA_vect          
PORTB_vect          
PORTC_vect          
PORTD_vect          
PORTE_vect          
SWI_vect      
Default_Handler
        B Default_Handler
        END

