/*======== startup_stm32f103.s =========== 
      - Định nghĩa vector table cho STM32F103 
      - Copy .data từ Flash vào RAM, clear .bss 
      - Gọi main(), vào vòng lặp vô hạn nếu main() trả về 
==========================================*/

.syntax unified
.cpu cortex-m3
.thumb

/* ========= Vector Table ========= */
.section .isr_vector, "a", %progbits
.align  2
.type   g_pfnVectors, %object
.size   g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word   _estack                 /* 0x00: Initial Stack Pointer */
    .word   Reset_Handler           /* 0x04: Reset Handler */
    .word   NMI_Handler             /* 0x08: NMI Handler */
    .word   HardFault_Handler       /* 0x0C: HardFault Handler */
    .word   MemManage_Handler       /* 0x10: MemManage Handler */
    .word   BusFault_Handler        /* 0x14: BusFault Handler */
    .word   UsageFault_Handler      /* 0x18: UsageFault Handler */
    .word   0                       /* 0x1C: Reserved */
    .word   0                       /* 0x20: Reserved */
    .word   0                       /* 0x24: Reserved */
    .word   0                       /* 0x28: Reserved */
    .word   SVC_Handler             /* 0x2C: SVCall Handler */
    .word   DebugMon_Handler        /* 0x30: DebugMon Handler */
    .word   0                       /* 0x34: Reserved */
    .word   PendSV_Handler          /* 0x38: PendSV Handler */
    .word   SysTick_Handler         /* 0x3C: SysTick Handler */

    /* External Interrupts (IRQ0 to IRQ28 shown here) */
    .word   Default_Handler         /* 0x40: WWDG */
    .word   Default_Handler         /* 0x44: PVD */
    .word   Default_Handler         /* 0x48: TAMPER */
    .word   Default_Handler         /* 0x4C: RTC */
    .word   Default_Handler         /* 0x50: FLASH */
    .word   Default_Handler         /* 0x54: RCC */
    .word   Default_Handler         /* 0x58: EXTI0 */
    .word   Default_Handler         /* 0x5C: EXTI1 */
    .word   Default_Handler         /* 0x60: EXTI2 */
    .word   Default_Handler         /* 0x64: EXTI3 */
    .word   Default_Handler         /* 0x68: EXTI4 */
    .word   DMA1_Channel1_IRQHandler         /* 0x6C: DMA1_Channel1 */
    .word   Default_Handler         /* 0x70: DMA1_Channel2 */
    .word   Default_Handler         /* 0x74: DMA1_Channel3 */
    .word   Default_Handler         /* 0x78: DMA1_Channel4 */
    .word   Default_Handler         /* 0x7C: DMA1_Channel5 */
    .word   Default_Handler         /* 0x80: DMA1_Channel6 */
    .word   Default_Handler         /* 0x84: DMA1_Channel7 */
    .word   ADC1_2_IRQHandler       /* 0x88: ADC1 and ADC2 */
    .word   Default_Handler         /* 0x8C: USB_HP_CAN_TX */
    .word   Default_Handler         /* 0x90: USB_LP_CAN_RX0 */
    .word   Default_Handler         /* 0x94: CAN_RX1 */
    .word   Default_Handler         /* 0x98: CAN_SCE */
    .word   Default_Handler         /* 0x9C: EXTI9_5 */
    .word   TIM2_IRQHandler         /* 0xA0: TIM2 */
    /* ... tiếp tục với các vector khác nếu cần thiết */

/* ========= Default Handler ========= */
.section .text.Default_Handler, "ax", %progbits
.weak   Default_Handler
.type   Default_Handler, %function
Default_Handler:
    b   Default_Handler

/* ========= Weak aliases ========= */
.weak   NMI_Handler
.set    NMI_Handler, Default_Handler

.weak   HardFault_Handler
.set    HardFault_Handler, Default_Handler

.weak   MemManage_Handler
.set    MemManage_Handler, Default_Handler

.weak   BusFault_Handler
.set    BusFault_Handler, Default_Handler

.weak   UsageFault_Handler
.set    UsageFault_Handler, Default_Handler

.weak   SVC_Handler
.set    SVC_Handler, Default_Handler

.weak   DebugMon_Handler
.set    DebugMon_Handler, Default_Handler

.weak   PendSV_Handler
.set    PendSV_Handler, Default_Handler

.weak   SysTick_Handler
.set    SysTick_Handler, Default_Handler

/* Interrupt weak handlers */
.weak   ADC1_2_IRQHandler
.set    ADC1_2_IRQHandler, Default_Handler

.weak   TIM2_IRQHandler
.set    TIM2_IRQHandler, Default_Handler

.weak   DMA1_Channel1_IRQHandler
.long   DMA1_Channel1_IRQHandler

/* ========= Reset Handler ========= */
.section .text.Reset_Handler, "ax", %progbits
.weak   Reset_Handler
.type   Reset_Handler, %function
Reset_Handler:
    /* Copy .data section from Flash to RAM */
    LDR   R0, =_sidata
    LDR   R1, =_sdata
    LDR   R2, =_edata
copy_data_loop:
    CMP   R1, R2
    ITT   LT
    LDRLT R3, [R0], #4
    STRLT R3, [R1], #4
    BLT   copy_data_loop

    /* Zero initialize .bss section */
    LDR   R0, =_sbss
    LDR   R1, =_ebss
    MOV   R2, #0
clear_bss_loop:
    CMP   R0, R1
    ITT   LT
    STRLT R2, [R0], #4
    BLT   clear_bss_loop

    /* Call main */
    BL    main

infinite_loop:
    B    infinite_loop

.size Reset_Handler, .-Reset_Handler
