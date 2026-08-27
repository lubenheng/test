;/**************************************************************************//**
; * @file     startup_ARMCM0plus.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM0plus Device
; * @version  V1.0.1
; * @date     23. July 2019
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------


;<h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Stack_Size      EQU      0x00000400

                AREA     STACK, NOINIT, READWRITE, ALIGN=3
__stack_limit
Stack_Mem       SPACE    Stack_Size
__initial_sp


;<h> Heap Configuration
;  <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Heap_Size       EQU      0x00000000

                AREA     HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE    Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA     RESET, DATA, READONLY
                EXPORT   __Vectors
                EXPORT   __Vectors_End
                EXPORT   __Vectors_Size

__Vectors       DCD      __initial_sp                        ;     Top of Stack
                DCD      Reset_Handler                       ;     Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      SVC_Handler                         ;  -5 SVCall Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      PendSV_Handler                      ;  -2 PendSV Handler
                DCD      SysTick_Handler                     ;  -1 SysTick Handler

                ; Interrupts
                DCD      PMU_IRQHandler                ;   0 Interrupt 0
                DCD      Code_Flash_IRQHandler             ;   1 Interrupt 1
                DCD      UART0_IRQHandler                  ;   2 Interrupt 2
                DCD      UART1_IRQHandler                  ;   3 Interrupt 3
                DCD      SPI0_IRQHandler                   ;   4 Interrupt 4
                DCD      SPI1_IRQHandler                   ;   5 Interrupt 5
                DCD      MCPWM0_IRQHandler                 ;   6 Interrupt 6
                DCD      MCPWM1_IRQHandler                 ;   7 Interrupt 7
                DCD      I2C0_IRQHandler                   ;   8 Interrupt 8
                DCD      I2C1_IRQHandler                   ;   9 Interrupt 9
								DCD      GPIO_A_IRQHandler                 ;	10 Interrupt 10
								DCD      GPIO_B_IRQHandler                 ;	11 Interrupt 11
                DCD      GPIO_C_IRQHandler                 ;	12 Interrupt 12
                DCD      TIM13_IRQHandler             		 ;	13 Interrupt 13
                DCD      TIM14_IRQHandler                  ;	14 Interrupt 14
                DCD      TIM15_IRQHandler                  ;	15 Interrupt 15
                DCD      TIM16_IRQHandler                  ;	16 Interrupt 16
                DCD      ADC_IRQHandler                    ;	17 Interrupt 17
                DCD      HALL_IRQHandler                   ;	18 Interrupt 18
                DCD      QEP_IRQHandler             			 ;	19 Interrupt 19
                DCD      DMA_IRQHandler                    ;	20 Interrupt 20
								DCD      Reserved21_IRQHandler             ;	21 Interrupt 21
                DCD      Reserved22_IRQHandler             ;	22 Interrupt 22
                DCD      CSA_IRQHandler                    ;	23 Interrupt 23
                DCD      Reserved24_IRQHandler             ;	24 Interrupt 24
                DCD      Reserved25_IRQHandler             ;	25 Interrupt 25
                DCD      Reserved26_IRQHandler             ;	26 Interrupt 26
                DCD      Reserved27_IRQHandler             ;	27 Interrupt 27
                DCD      Reserved28_IRQHandler             ;	28 Interrupt 28
                DCD      Reserved29_IRQHandler             ;	29 Interrupt 29
                DCD      Reserved30_IRQHandler             ;	30 Interrupt 30
								DCD      Reserved31_IRQHandler             ;	31 Interrupt 31

__Vectors_End
__Vectors_Size  EQU      __Vectors_End - __Vectors


                AREA     |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT   Reset_Handler             [WEAK]
                IMPORT   SystemInit
                IMPORT   __main

                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__main
                BX       R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP
								 
Default_Handler PROC
                EXPORT   PMU_IRQHandler        [WEAK]
                EXPORT   Code_Flash_IRQHandler     [WEAK]
                EXPORT   UART0_IRQHandler          [WEAK]
                EXPORT   UART1_IRQHandler          [WEAK]
                EXPORT   SPI0_IRQHandler           [WEAK]
                EXPORT   SPI1_IRQHandler           [WEAK]
                EXPORT   MCPWM0_IRQHandler         [WEAK]
                EXPORT   MCPWM1_IRQHandler         [WEAK]
                EXPORT   I2C0_IRQHandler           [WEAK]
                EXPORT   I2C1_IRQHandler           [WEAK]
                EXPORT   GPIO_A_IRQHandler         [WEAK]
                EXPORT   GPIO_B_IRQHandler         [WEAK]
                EXPORT   GPIO_C_IRQHandler         [WEAK]
                EXPORT   TIM13_IRQHandler    			 [WEAK]
                EXPORT   TIM14_IRQHandler    			 [WEAK]
                EXPORT   TIM15_IRQHandler   		   [WEAK]
                EXPORT   TIM16_IRQHandler          [WEAK]
                EXPORT   ADC_IRQHandler            [WEAK]
                EXPORT   HALL_IRQHandler           [WEAK]
                EXPORT   QEP_IRQHandler            [WEAK]
                EXPORT   DMA_IRQHandler            [WEAK]
                EXPORT   Reserved21_IRQHandler     [WEAK]
                EXPORT   Reserved22_IRQHandler     [WEAK]
                EXPORT   CSA_IRQHandler            [WEAK]
                EXPORT   Reserved24_IRQHandler     [WEAK]
                EXPORT   Reserved25_IRQHandler     [WEAK]
                EXPORT   Reserved26_IRQHandler     [WEAK]
                EXPORT   Reserved27_IRQHandler     [WEAK]
								EXPORT   Reserved28_IRQHandler     [WEAK]
								EXPORT   Reserved29_IRQHandler     [WEAK]
								EXPORT   Reserved30_IRQHandler     [WEAK]
								EXPORT   Reserved31_IRQHandler 	   [WEAK]

PMU_IRQHandler        
Code_Flash_IRQHandler     
UART0_IRQHandler          
UART1_IRQHandler          
SPI0_IRQHandler           
SPI1_IRQHandler           
MCPWM0_IRQHandler         
MCPWM1_IRQHandler         
I2C0_IRQHandler           
I2C1_IRQHandler           
GPIO_A_IRQHandler  
GPIO_B_IRQHandler  
GPIO_C_IRQHandler  
TIM13_IRQHandler  
TIM14_IRQHandler  
TIM15_IRQHandler  
TIM16_IRQHandler  
ADC_IRQHandler  
HALL_IRQHandler  
QEP_IRQHandler  
DMA_IRQHandler  
Reserved21_IRQHandler  
Reserved22_IRQHandler  
CSA_IRQHandler  
Reserved24_IRQHandler  
Reserved25_IRQHandler  
Reserved26_IRQHandler  
Reserved27_IRQHandler  
Reserved28_IRQHandler  
Reserved29_IRQHandler  
Reserved30_IRQHandler  
Reserved31_IRQHandler  

                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END