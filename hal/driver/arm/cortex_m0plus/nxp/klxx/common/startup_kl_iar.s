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
    SECTION .text  : CODE
    EXTERN  __iar_program_start
    EXPORT  __startup
__startup
        CPSID   i                       ; mask interrupts
        BL      __iar_program_start     ; call the C code
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
        DCD     NMI_Handler                 ; 2  NMI Handler
        DCD     HardFault_Handler           ; 3  Hard Fault Handler
        DCD     0                           ; 4  MPU Fault Handler
        DCD     0                           ; 5  Bus Fault Handler
        DCD     0                           ; 6  Usage Fault Handler
        DCD     0                           ; 7  Reserved
        DCD     0                           ; 8  Reserved
        DCD     0                           ; 9  Reserved
        DCD     0                           ; 10 Reserved
        DCD     SVC_Handler                 ; 11 SVCall Handler
        DCD     0                           ; 12 Debug Monitor Handler
        DCD     0                           ; 13 Reserved
        DCD     PendSV_Handler              ; 14 PendSV Handler
        DCD     SysTick_Handler             ; 15 SysTick Handler
        ; External Interrupts
        DCD     PIN_INT0_vect               ; 0:   Pin Interrupt Vector 0
        DCD     PIN_INT1_vect               ; 1:   Pin Interrupt Vector 1
        DCD     PIN_INT2_vect               ; 2:   Pin Interrupt Vector 2
        DCD     PIN_INT3_vect               ; 3:   Pin Interrupt Vector 3
        DCD     PIN_INT4_vect               ; 4:   Pin Interrupt Vector 4
        DCD     PIN_INT5_vect               ; 5:   Pin Interrupt Vector 5
        DCD     PIN_INT6_vect               ; 6:   Pin Interrupt Vector 6
        DCD     PIN_INT7_vect               ; 7:   Pin Interrupt Vector 7
        DCD     PIN_INT8_vect               ; 8:   Pin Interrupt Vector 8
        DCD     PIN_INT9_vect               ; 9:   Pin Interrupt Vector 9
        DCD     PIN_INT10_vect              ; 10:  Pin Interrupt Vector 10
        DCD     PIN_INT11_vect              ; 11:  Pin Interrupt Vector 11
        DCD     I2C0_vect                   ; 12:  I2C0 Interrupt Vector
        DCD     TIMER0_vect                   ; 13:  16Bits TIMER0 Interrupt Vector
        DCD     TIMER1_vect                   ; 14:  16Bits TIMER1 Interrupt Vector
        DCD     TIMER2_vect                   ; 15:  32Bits TIMER2 Interrupt Vector
        DCD     TIMER3_vect                   ; 16:  32Bits TIMER3 Interrupt Vector
        DCD     SPI0_vect                   ; 17:  SPI1 Interrupt Vector
        DCD     USART0_vect                 ; 18:  USART0 Interrupt Vector
        DCD     USART1_vect                 ; 19:  USART1 Interrupt Vector
        DCD     COMPARATOR                  ; 20:  Comparator 0/1 interrupt
        DCD     ADC0_vect                   ; 21:  ADC0 Interrupt Vector
        DCD     WDT_vect                    ; 22:  WDT Interrupt Vector
        DCD     BOD_vect                    ; 23:  BOD Interrupt Vector
        DCD     0                           ; 24:  Reserved
        DCD     GINT0_vect                  ; 25:  GPIO Group0 Interrupt Vector 
        DCD     GINT1_vect                  ; 26:  GPIO Group1 Interrupt Vector 
        DCD     GINT2_vect                  ; 27:  GPIO Group2 Interrupt Vector 
        DCD     0                           ; 28:  SIGNAL Output 28
        DCD     DMA_vect                    ; 29:  DMA Interrupt Vector
        DCD     RTC_vect                    ; 30:  RTC Interrupt Vector
        DCD     0                           ; 31:  SIGNAL Output 31

;;------------------------------------------------------------------------------
;; Default interrupt handlers.
;;------------------------------------------------------------------------------
    THUMB
        PUBWEAK NMI_Handler         
        PUBWEAK HardFault_Handler                   
        PUBWEAK SVC_Handler                        
        PUBWEAK PendSV_Handler      
        PUBWEAK SysTick_Handler

        PUBWEAK     PIN_INT0_vect               ; 0:   Pin Interrupt Vector 0
        PUBWEAK     PIN_INT1_vect               ; 1:   Pin Interrupt Vector 1
        PUBWEAK     PIN_INT2_vect               ; 2:   Pin Interrupt Vector 2
        PUBWEAK     PIN_INT3_vect               ; 3:   Pin Interrupt Vector 3
        PUBWEAK     PIN_INT4_vect               ; 4:   Pin Interrupt Vector 4
        PUBWEAK     PIN_INT5_vect               ; 5:   Pin Interrupt Vector 5
        PUBWEAK     PIN_INT6_vect               ; 6:   Pin Interrupt Vector 6
        PUBWEAK     PIN_INT7_vect               ; 7:   Pin Interrupt Vector 7
        PUBWEAK     PIN_INT8_vect               ; 8:   Pin Interrupt Vector 8
        PUBWEAK     PIN_INT9_vect               ; 9:   Pin Interrupt Vector 9
        PUBWEAK     PIN_INT10_vect              ; 10:  Pin Interrupt Vector 10
        PUBWEAK     PIN_INT11_vect              ; 11:  Pin Interrupt Vector 11
        PUBWEAK     I2C0_vect                   ; 12:  I2C0 Interrupt Vector
        PUBWEAK     TIMER0_vect                   ; 13:  16Bits TIMER0 Interrupt Vector
        PUBWEAK     TIMER1_vect                   ; 14:  16Bits TIMER1 Interrupt Vector
        PUBWEAK     TIMER2_vect                   ; 15:  32Bits TIMER2 Interrupt Vector
        PUBWEAK     TIMER3_vect                   ; 16:  32Bits TIMER3 Interrupt Vector
        PUBWEAK     SPI0_vect                   ; 17:  SPI1 Interrupt Vector
        PUBWEAK     USART0_vect                 ; 18:  USART0 Interrupt Vector
        PUBWEAK     USART1_vect                 ; 19:  USART1 Interrupt Vector
        PUBWEAK     COMPARATOR                  ; 20:  Comparator 0/1 interrupt
        PUBWEAK     ADC0_vect                   ; 21:  ADC0 Interrupt Vector
        PUBWEAK     WDT_vect                    ; 22:  WDT Interrupt Vector
        PUBWEAK     BOD_vect                    ; 23:  BOD Interrupt Vector
        PUBWEAK     GINT0_vect                  ; 25:  GPIO Group0 Interrupt Vector 
        PUBWEAK     GINT1_vect                  ; 26:  GPIO Group1 Interrupt Vector 
        PUBWEAK     GINT2_vect                  ; 27:  GPIO Group2 Interrupt Vector 
        PUBWEAK     DMA_vect                    ; 29:  DMA Interrupt Vector
        PUBWEAK     RTC_vect                    ; 30:  RTC Interrupt Vector

    SECTION .text:CODE:REORDER(1)
      
NMI_Handler         
HardFault_Handler          
SVC_Handler                    
PendSV_Handler            
SysTick_Handler      

PIN_INT0_vect               ; 0:   Pin Interrupt Vector 0
PIN_INT1_vect               ; 1:   Pin Interrupt Vector 1
PIN_INT2_vect               ; 2:   Pin Interrupt Vector 2
PIN_INT3_vect               ; 3:   Pin Interrupt Vector 3
PIN_INT4_vect               ; 4:   Pin Interrupt Vector 4
PIN_INT5_vect               ; 5:   Pin Interrupt Vector 5
PIN_INT6_vect               ; 6:   Pin Interrupt Vector 6
PIN_INT7_vect               ; 7:   Pin Interrupt Vector 7
PIN_INT8_vect               ; 8:   Pin Interrupt Vector 8
PIN_INT9_vect               ; 9:   Pin Interrupt Vector 9
PIN_INT10_vect              ; 10:  Pin Interrupt Vector 10
PIN_INT11_vect              ; 11:  Pin Interrupt Vector 11
I2C0_vect                   ; 12:  I2C0 Interrupt Vector
TIMER0_vect                   ; 13:  16Bits TIMER0 Interrupt Vector
TIMER1_vect                   ; 14:  16Bits TIMER1 Interrupt Vector
TIMER2_vect                   ; 15:  32Bits TIMER2 Interrupt Vector
TIMER3_vect                   ; 16:  32Bits TIMER3 Interrupt Vector
SPI0_vect                   ; 17:  SPI1 Interrupt Vector
USART0_vect                 ; 18:  USART0 Interrupt Vector
USART1_vect                 ; 19:  USART1 Interrupt Vector
COMPARATOR                  ; 20:  Comparator 0/1 interrupt
ADC0_vect                   ; 21:  ADC0 Interrupt Vector
WDT_vect                    ; 22:  WDT Interrupt Vector
BOD_vect                    ; 23:  BOD Interrupt Vector
GINT0_vect                  ; 25:  GPIO Group0 Interrupt Vector 
GINT1_vect                  ; 26:  GPIO Group1 Interrupt Vector 
GINT2_vect                  ; 27:  GPIO Group2 Interrupt Vector 
DMA_vect                    ; 29:  DMA Interrupt Vector
RTC_vect                    ; 30:  RTC Interrupt Vector

Default_Handler
        B Default_Handler
        END

