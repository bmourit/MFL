// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include <cstdint>
#include <cstddef>

#include "RCU.hpp"
#include "gd32f303re.h"

extern "C" {
    extern const void* _estack;
}

extern "C" {
// External references to symbols defined in the linker script
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

// Function prototypes
void Reset_Handler();
void Default_Handler();
void __libc_init_array();
int main();

// Weakly define interrupt handlers to Default_Handler
#define WEAK_ALIAS(x) __attribute__ ((weak, alias(#x)))

// Cortex-M4 processor exceptions handlers
void NMI_Handler(void) WEAK_ALIAS(Default_Handler);
void HardFault_Handler(void) WEAK_ALIAS(Default_Handler);
void MemManage_Handler(void) WEAK_ALIAS(Default_Handler);
void BusFault_Handler(void) WEAK_ALIAS(Default_Handler);
void UsageFault_Handler(void) WEAK_ALIAS(Default_Handler);
void SVC_Handler(void) WEAK_ALIAS(Default_Handler);
void DebugMon_Handler(void) WEAK_ALIAS(Default_Handler);
void PendSV_Handler(void) WEAK_ALIAS(Default_Handler);
void SysTick_Handler(void) WEAK_ALIAS(Default_Handler);

// Specific interrupt handlers (weakly defined to Default_Handler)
void WWDGT_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void LVD_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TAMPER_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void RTC_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void FMC_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void RCU_CTC_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI3_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI4_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel3_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel4_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel5_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA0_Channel6_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void ADC0_1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USBD_HP_CAN0_TX_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USBD_LP_CAN0_RX0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void CAN0_RX1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void CAN0_EWMC_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI5_9_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER0_BRK_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER0_UP_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER0_TRG_CMT_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER0_Channel_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER3_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void I2C0_EV_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void I2C0_ER_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void I2C1_EV_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void I2C1_ER_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void SPI0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void SPI1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USART0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USART1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USART2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXTI10_15_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void RTC_Alarm_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void USBD_WKUP_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER7_BRK_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER7_UP_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER7_TRG_CMT_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER7_Channel_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void ADC2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void EXMC_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void SDIO_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER4_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void SPI2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void UART3_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void UART4_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER5_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void TIMER6_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA1_Channel0_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA1_Channel1_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA1_Channel2_IRQHandler(void) WEAK_ALIAS(Default_Handler);
void DMA1_Channel3_4_IRQHandler(void) WEAK_ALIAS(Default_Handler);

// Vector table
// Define the vector table as an array of const void*
__attribute__((section(".isr_vector")))
const void* g_pfnVectors[] = {
    _estack,                                                         // Initial stack pointer
    reinterpret_cast<const void*>(Reset_Handler),                    // Reset handler
    reinterpret_cast<const void*>(NMI_Handler),                      // NMI handler
    reinterpret_cast<const void*>(HardFault_Handler),                // Hard fault handler
    reinterpret_cast<const void*>(MemManage_Handler),                // Memory management handler
    reinterpret_cast<const void*>(BusFault_Handler),                 // Bus fault handler
    reinterpret_cast<const void*>(UsageFault_Handler),               // Usage fault handler
    reinterpret_cast<const void*>(0),                                // Reserved
    reinterpret_cast<const void*>(0),                                // Reserved
    reinterpret_cast<const void*>(0),                                // Reserved
    reinterpret_cast<const void*>(0),                                // Reserved
    reinterpret_cast<const void*>(SVC_Handler),                      // SVCall handler
    reinterpret_cast<const void*>(DebugMon_Handler),                 // Debug monitor handler
    reinterpret_cast<const void*>(0),                                // Reserved
    reinterpret_cast<const void*>(PendSV_Handler),                   // PendSV handler
    reinterpret_cast<const void*>(SysTick_Handler),                  // SysTick handler
    reinterpret_cast<const void*>(WWDGT_IRQHandler),                 // Window Watchdog Timer
    reinterpret_cast<const void*>(LVD_IRQHandler),                   // Low Voltage Detector
    reinterpret_cast<const void*>(TAMPER_IRQHandler),                // Tamper
    reinterpret_cast<const void*>(RTC_IRQHandler),                   // RTC
    reinterpret_cast<const void*>(FMC_IRQHandler),                   // Flash Memory Controller
    reinterpret_cast<const void*>(RCU_CTC_IRQHandler),               // Reset and Clock Control
    reinterpret_cast<const void*>(EXTI0_IRQHandler),                 // External Interrupt 0
    reinterpret_cast<const void*>(EXTI1_IRQHandler),                 // External Interrupt 1
    reinterpret_cast<const void*>(EXTI2_IRQHandler),                 // External Interrupt 2
    reinterpret_cast<const void*>(EXTI3_IRQHandler),                 // External Interrupt 3
    reinterpret_cast<const void*>(EXTI4_IRQHandler),                 // External Interrupt 4
    reinterpret_cast<const void*>(DMA0_Channel0_IRQHandler),         // DMA Channel 0
    reinterpret_cast<const void*>(DMA0_Channel1_IRQHandler),         // DMA Channel 1
    reinterpret_cast<const void*>(DMA0_Channel2_IRQHandler),         // DMA Channel 2
    reinterpret_cast<const void*>(DMA0_Channel3_IRQHandler),         // DMA Channel 3
    reinterpret_cast<const void*>(DMA0_Channel4_IRQHandler),         // DMA Channel 4
    reinterpret_cast<const void*>(DMA0_Channel5_IRQHandler),         // DMA Channel 5
    reinterpret_cast<const void*>(DMA0_Channel6_IRQHandler),         // DMA Channel 6
    reinterpret_cast<const void*>(ADC0_1_IRQHandler),                // ADC0 and ADC1
    reinterpret_cast<const void*>(USBD_HP_CAN0_TX_IRQHandler),       // USB Device High Priority / CAN0 TX
    reinterpret_cast<const void*>(USBD_LP_CAN0_RX0_IRQHandler),      // USB Device Low Priority / CAN0 RX0
    reinterpret_cast<const void*>(CAN0_RX1_IRQHandler),              // CAN0 RX1
    reinterpret_cast<const void*>(CAN0_EWMC_IRQHandler),             // CAN0 Error Warning
    reinterpret_cast<const void*>(EXTI5_9_IRQHandler),               // External Interrupts 5-9
    reinterpret_cast<const void*>(TIMER0_BRK_IRQHandler),            // Timer 0 Break
    reinterpret_cast<const void*>(TIMER0_UP_IRQHandler),             // Timer 0 Update
    reinterpret_cast<const void*>(TIMER0_TRG_CMT_IRQHandler),        // Timer 0 Trigger and Commutation
    reinterpret_cast<const void*>(TIMER0_Channel_IRQHandler),        // Timer 0 Channel
    reinterpret_cast<const void*>(TIMER1_IRQHandler),                // Timer 1
    reinterpret_cast<const void*>(TIMER2_IRQHandler),                // Timer 2
    reinterpret_cast<const void*>(TIMER3_IRQHandler),                // Timer 3
    reinterpret_cast<const void*>(I2C0_EV_IRQHandler),               // I2C0 Event
    reinterpret_cast<const void*>(I2C0_ER_IRQHandler),               // I2C0 Error
    reinterpret_cast<const void*>(I2C1_EV_IRQHandler),               // I2C1 Event
    reinterpret_cast<const void*>(I2C1_ER_IRQHandler),               // I2C1 Error
    reinterpret_cast<const void*>(SPI0_IRQHandler),                  // SPI0
    reinterpret_cast<const void*>(SPI1_IRQHandler),                  // SPI1
    reinterpret_cast<const void*>(USART0_IRQHandler),                // USART0
    reinterpret_cast<const void*>(USART1_IRQHandler),                // USART1
    reinterpret_cast<const void*>(USART2_IRQHandler),                // USART2
    reinterpret_cast<const void*>(EXTI10_15_IRQHandler),             // External Interrupts 10-15
    reinterpret_cast<const void*>(RTC_Alarm_IRQHandler),             // RTC Alarm
    reinterpret_cast<const void*>(USBD_WKUP_IRQHandler),             // USB Device Wakeup
    reinterpret_cast<const void*>(TIMER7_BRK_IRQHandler),            // Timer 7 Break
    reinterpret_cast<const void*>(TIMER7_UP_IRQHandler),             // Timer 7 Update
    reinterpret_cast<const void*>(TIMER7_TRG_CMT_IRQHandler),        // Timer 7 Trigger and Commutation
    reinterpret_cast<const void*>(TIMER7_Channel_IRQHandler),        // Timer 7 Channel
    reinterpret_cast<const void*>(ADC2_IRQHandler),                  // ADC2
    reinterpret_cast<const void*>(EXMC_IRQHandler),                  // External Memory Controller
    reinterpret_cast<const void*>(SDIO_IRQHandler),                  // SDIO
    reinterpret_cast<const void*>(TIMER4_IRQHandler),                // Timer 4
    reinterpret_cast<const void*>(SPI2_IRQHandler),                  // SPI2
    reinterpret_cast<const void*>(UART3_IRQHandler),                 // UART3
    reinterpret_cast<const void*>(UART4_IRQHandler),                 // UART4
    reinterpret_cast<const void*>(TIMER5_IRQHandler),                // Timer 5
    reinterpret_cast<const void*>(TIMER6_IRQHandler),                // Timer 6
    reinterpret_cast<const void*>(DMA1_Channel0_IRQHandler),         // DMA1 Channel 0
    reinterpret_cast<const void*>(DMA1_Channel1_IRQHandler),         // DMA1 Channel 1
    reinterpret_cast<const void*>(DMA1_Channel2_IRQHandler),         // DMA1 Channel 2
    reinterpret_cast<const void*>(DMA1_Channel3_4_IRQHandler),       // DMA1 Channels 3 and 4
};

// Reset handler
void Reset_Handler(void) {
    // Copy data section from Flash to SRAM
    uint32_t* src = &_sidata;
    uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // Zero-initialize the BSS section
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // Call system startup
    RCU_DEVICE.system_startup();

    // Call C++ global constructors
    __libc_init_array();

    // Call the main function
    main();

    // If main returns, loop indefinitely
    while (true) {}
}

// Default interrupt handler (infinite loop)
void Default_Handler(void) {
    while (1) {}
}

} // extern "C"
