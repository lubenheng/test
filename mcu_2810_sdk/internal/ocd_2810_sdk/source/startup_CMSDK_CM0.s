
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp            ; Top of Stack
                DCD     Reset_Handler           ; -15
                DCD     NMI_Handler             ; -14
                DCD     HardFault_Handler       ; -13
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     SVC_Handler             ; -5
                DCD     0                       ; 
                DCD     0                       ; 
                DCD     PendSV_Handler          ; -2
                DCD     SysTick_Handler         ; -1
                DCD     int0_Handler            ; 0
                DCD     int1_Handler            ; 1
                DCD     int2_Handler            ; 2
                DCD     int3_Handler            ; 3
                DCD     int4_Handler            ; 4
                DCD     pin_Handler             ; 5
                DCD     rtc_Handler             ; 6
                DCD     uart0_Handler           ; 7
                DCD     uart1_Handler           ; 8
                DCD     i2c0_Handler            ; 9
                DCD     tim0_Handler            ; 10
                DCD     tim1_Handler            ; 11
                DCD     tim2_Handler            ; 12
                DCD     tim3_Handler            ; 13
                DCD     tim4_Handler            ; 14
                DCD     fgif_Handler            ; 15
                DCD     dcif_Handler            ; 16
                DCD     adc_Handler             ; 17
                DCD     cmp0if_Handler          ; 18
                DCD     cmp1if_Handler          ; 19
                DCD     cmp2if_Handler          ; 20
                DCD     cmp3if_Handler          ; 21
                DCD     cmp4if_Handler          ; 22
                DCD     tim5_Handler            ; 23
                DCD     spi_Handler             ; 24
                DCD     tsd_Handler             ; 25
                DCD     uvlo_Handler            ; 26
                DCD     fault_Handler           ; 27
                DCD     i2c1_Handler            ; 28
                DCD     foc_Handler             ; 29
                DCD     swd_Handler             ; 30
                DCD     tim6_Handler            ; 31
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                ; Initialise at least r8, r9 to avoid X in tests
                ; Only important for simulation where X can cause
                ; unexpected core behaviour
                MOV     R8, R0
                MOV     R9, R8
                BLX     R0
                LDR     R0, =__main
                BX      R0
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
                EXPORT int0_Handler               [WEAK]
                EXPORT int1_Handler               [WEAK]
                EXPORT int2_Handler               [WEAK]
                EXPORT int3_Handler               [WEAK]
                EXPORT int4_Handler               [WEAK]
                EXPORT pin_Handler                [WEAK]
                EXPORT rtc_Handler                [WEAK]
                EXPORT uart0_Handler              [WEAK]
                EXPORT uart1_Handler              [WEAK]
                EXPORT i2c0_Handler               [WEAK]
                EXPORT tim0_Handler               [WEAK]
                EXPORT tim1_Handler               [WEAK]
                EXPORT tim2_Handler               [WEAK]
                EXPORT tim3_Handler               [WEAK]
                EXPORT tim4_Handler               [WEAK]
                EXPORT fgif_Handler               [WEAK]
                EXPORT dcif_Handler               [WEAK]
                EXPORT adc_Handler                [WEAK]
                EXPORT cmp0if_Handler             [WEAK]
                EXPORT cmp1if_Handler             [WEAK]
                EXPORT cmp2if_Handler             [WEAK]
                EXPORT cmp3if_Handler             [WEAK]
                EXPORT cmp4if_Handler             [WEAK]
                EXPORT tim5_Handler               [WEAK]
                EXPORT spi_Handler                [WEAK]
                EXPORT tsd_Handler                [WEAK]
                EXPORT uvlo_Handler               [WEAK]
                EXPORT fault_Handler              [WEAK]
			    EXPORT i2c1_Handler               [WEAK]
				EXPORT foc_Handler                [WEAK]
			    EXPORT swd_Handler                [WEAK]
				EXPORT tim6_Handler               [WEAK]	

int0_Handler   
int1_Handler   
int2_Handler   
int3_Handler   
int4_Handler   
pin_Handler   
rtc_Handler   
uart0_Handler   
uart1_Handler   
i2c0_Handler
tim0_Handler
tim1_Handler      
tim2_Handler       
tim3_Handler 
tim4_Handler    
fgif_Handler    
dcif_Handler     
adc_Handler
cmp0if_Handler
cmp1if_Handler
cmp2if_Handler
cmp3if_Handler
cmp4if_Handler
tim5_Handler
spi_Handler     
tsd_Handler     
uvlo_Handler    
fault_Handler
i2c1_Handler
foc_Handler
swd_Handler
tim6_Handler

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
