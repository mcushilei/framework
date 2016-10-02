;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.86
; By        : Jean J. Labrosse
;             Brian Nagel
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : IAR EWARM
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning                               ; External references
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSIntNesting
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook
    EXTERN  OSCriticalNesting
    EXTERN  SVC_Process

    PUBLIC  CALL_SVC1
    PUBLIC  CALL_SVC2
    PUBLIC  TASK_SW
    PUBLIC  ENTER_CRITICAL
    PUBLIC  EXIT_CRITICAL
    PUBLIC  SET_INTERRUPT_MASK
    PUBLIC  CHANGE_CPU_PRIVILEGE
    PUBLIC  SVC_Handler
    PUBLIC  PendSV_Handler
    PUBLIC  OSStartHighRdy
    PUBLIC  OSCtxSw
    PUBLIC  OSIntCtxSw
    PUBLIC  DISABLE_ALL_INT

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

NVIC_INT_CTRL       EQU     0xE000ED04              ; Interrupt control state register.
NVIC_PENDSVSET      EQU     0x10000000              ; Value to trigger PendSV exception.
NVIC_PENDSV_REG     EQU     0xE000ED22              ; System priority register (priority 14).
NVIC_PENDSV_PRI     EQU           0xFF              ; PendSV priority value (lowest).
NVIC_SVC_REG        EQU     0xE000ED1F  
NVIC_SVC_PRI 	    EQU           0x00  
NVIC_SYSTICK_REG    EQU     0xE000ED23

INT_PRIO_MASK       EQU    (1 << (8 - 5 + 0))       ; 屏蔽大于等于此值优先级中断，这里屏蔽所有中断
                                                    ; 5：LPC17xx使用5个位表示优先级
                                                    ; 0：表示子优先级的位数为0

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    SECTION .text:CODE:NOROOT(2)
;	AREA |.text|, CODE, READONLY, ALIGN=2           ;AREA |.text| 选择段 |.text|。
                                          	        ;CODE表示代码段，READONLY表示只读（缺省）
                                          	        ;ALIGN=2表示4字节对齐。若ALIGN=n，这2^n对齐
	THUMB                                           ;Thumb 代码
	REQUIRE8                                        ;指定当前文件要求堆栈八字节对齐
	PRESERVE8                                       ;令指定当前文件保持堆栈八字节对齐




;********************************************************************************************************
;  请求1号SVC调用
;********************************************************************************************************

CALL_SVC1
    SVC     #1
    BX      LR

;********************************************************************************************************
;  请求2号SVC调用
;********************************************************************************************************

CALL_SVC2
    SVC     #2
    BX      LR

;********************************************************************************************************
;  SVC_Handler
;********************************************************************************************************

SVC_Handler
    ;判断SVC中断前使用的哪个堆栈
    TST     LR, #0x04                               ;按位与，并更新Z标志
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP
    PUSH    {LR}
    BL      SVC_Process    
    POP     {PC}

;********************************************************************************************************
;                                              触发任务调度
;                                           void TASK_SW(void)
;********************************************************************************************************

TASK_SW	    	    	    	    	            
	LDR     R0, =NVIC_INT_CTRL                      ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                                              进入临界区
;                                         void ENTER_CRITICAL(void)
;********************************************************************************************************

ENTER_CRITICAL	    	    	    	    	    ;关中断
    LDR 	R1, =INT_PRIO_MASK                               
    MSR 	BASEPRI, R1
                                                                                  	         	    	    
	LDR 	R1, =OSCriticalNesting	                ;OSCriticalNesting++
	LDR  	R2, [R1]
	CMP		R2, #255
	IT      NE
    ADDNE   R2, R2, #1
	STR  	R2, [R1]
	
    BX      LR

;********************************************************************************************************
;                                              退出临界区
;                                         void EXIT_CRITICAL(void)
;********************************************************************************************************

EXIT_CRITICAL	    	    	    	    	    ;开中断
	LDR     R1, =OSCriticalNesting                  ;OSCriticalNesting--
	LDR     R2, [R1]
	CMP		R2, #0
	IT      NE
    SUBNE   R2, R2, #1
	STR     R2, [R1]
	
    MOV     R1, #0	      
	CMP     R2, #0	    	                        ;如果 OSCriticalNesting = 0, 开中断                               
	IT      EQ
    MSREQ	BASEPRI, R1	                         	          
	
    BX      LR

;********************************************************************************************************
;                                            设定处理器中断屏蔽等级
;                                      INT32U SET_INTERRUPT_MASK(INT32U)
;********************************************************************************************************

SET_INTERRUPT_MASK
	MRS     R1, BASEPRI                             ;保存当前 BASEPRI
    MSR 	BASEPRI, R0
    MOV     R0, R1                                  ;返回 BASEPRI
    BX      LR
    
;********************************************************************************************************
;                                              改变处理器等级
;                                       void CHANGE_CPU_PRIVILEGE(INT8U)
;********************************************************************************************************

CHANGE_CPU_PRIVILEGE	    	    	    	    	        
    CMP     R0, #2
    IT      LS
    MSRLS 	CONTROL, R0
    BX      LR
    
;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set OSRunning to TRUE;
;              d) Trigger PendSV exception;
;              e) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************

OSStartHighRdy
    LDR     R0, =NVIC_SVC_REG                       ; Set the SVC exception priority
    LDR     R1, =NVIC_SVC_PRI
    STRB    R1, [R0]
    
    LDR     R0, =NVIC_PENDSV_REG                    ; Set the PEND exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]
    
    LDR     R0, =NVIC_SYSTICK_REG                   ; Set the SYSTICK exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]
    
    MOVS    R0, #0                                  ; Set the PSP to 0 for initial context switch call
    MSR     PSP, R0
    
    LDR     R0, =OSRunning                          ; OSRunning = TRUE
    MOVS    R1, #1
    STRB    R1, [R0]
    
    LDR     R0, =NVIC_INT_CTRL                      ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    
    LDR     R1, =OSCriticalNesting                  ; OSCriticalNesting = 0
    MOV     R0, #0
    STR     R0, [R1]
    
    MOV     R0, #0	                                ; enable interrupts at processor level      
    MSR	    BASEPRI, R0	                            
                                                    
        
OSStartHang
    B       OSStartHang                             ; should never get here

;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************

OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;********************************************************************************************************

OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                      ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 (by OSStartHighRdy() above) then skip (goto d) the saving 
;                 part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

PendSV_Handler
	LDR 	R0, =INT_PRIO_MASK                      ; Prevent interruption during context switch
    MSR 	BASEPRI, R0                             
	
    MRS     R0, PSP                                 ; PSP is process stack pointer
    CBZ     R0, OS_CPU_PendSVHandler_nosave         ; Skip register save the first time
    	    	    	    	    	            ; 启动第一个任务时，当然没有前一个任务堆栈
    SUBS    R0, R0, #0x20                           ; Save remaining regs r4-11 on process stack
    STM     R0, {R4 - R11}
    
	;SUBS    R0, R0, #0x04                           ; 保存当前 OSCriticalNesting 到任务堆栈
    ;LDR     R1, =OSCriticalNesting
	;LDR     R1, [R1]
	;STR     R1, [R0]    	    	    	    	    	    

	LDR     R1, =OSTCBCur                           ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                ; R0 is SP of process being switched out
                                                    ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave
    PUSH    {R14}                                   ; Save LR exc_return value
    LDR     R0, =OSTaskSwHook                       ; OSTaskSwHook();
    BLX     R0
    POP     {R14}
    
    LDR     R0, =OSPrioCur                          ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]
    
    LDR     R0, =OSTCBCur                           ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]
    LDR     R0, [R2]                                ; R0 = SP(SP = OSTCBHighRdy->OSTCBStkPtr)
    
    ;LDR     R1, =OSCriticalNesting                  ; 从新任务堆栈取出中断层数值保存到 OSCriticalNesting
    ;LDR     R2, [R0]                                ; 任务第一次运行前，任务堆栈已被 OSTaskStkInit() 初始化过
    ;STR     R2, [R1]    	    	    	    	    	    
    ;ADDS    R0, R0, #0x04
    
    LDM     R0, {R4-R11}                            ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20                       
    MSR     PSP, R0                                 ; Load PSP with new process SP
    ORR     LR, LR, #0x04                           ; Ensure exception return uses process stack    
        	                                                                            
    MOV     R0, #0x1                                ; 设置 CONTROL 寄存器，让用户程序切入用户级
    MSR     CONTROL, R0         
    
    MOV     R0, #0 
    MSR 	BASEPRI, R0      	    	            ; 从 R3 恢复 BASEPRI   	    
    BX      LR                                      ; Exception return will restore remaining context




DISABLE_ALL_INT
    LDR 	R1, =INT_PRIO_MASK                               
    MSR 	BASEPRI, R1
	BX      LR
    	
    
    END

