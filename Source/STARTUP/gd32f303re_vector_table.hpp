// gd32f30x STARTUP peripheral register access in C++
// Copyright (c) B. Mourit <bnmguy@gmail.com
// All rights reserved.

#include <cstdint>

#define DEFINE_DEFAULT_ISR(name) \
    extern "C" \
    __attribute__((interrupt)) \
    __attribute__((weak)) \
    __attribute__((noreturn)) \
    void name() { \
        while(true); \
    }

DEFINE_DEFAULT_ISR(Default_Handler)
DEFINE_DEFAULT_ISR(NMI_Handler)
DEFINE_DEFAULT_ISR(HardFault_Handler)
DEFINE_DEFAULT_ISR(MemManage_Handler)
DEFINE_DEFAULT_ISR(BusFault_Handler)
DEFINE_DEFAULT_ISR(UsageFault_Handler)
DEFINE_DEFAULT_ISR(SVC_Handler)
DEFINE_DEFAULT_ISR(DebugMon_Handler)
DEFINE_DEFAULT_ISR(PendSV_Handler)
DEFINE_DEFAULT_ISR(SysTick_Handler)
DEFINE_DEFAULT_ISR(WWDGT_IRQHandler)
DEFINE_DEFAULT_ISR(LVD_IRQHandler)
DEFINE_DEFAULT_ISR(TAMPER_IRQHandler)
DEFINE_DEFAULT_ISR(RTC_IRQHandler)
DEFINE_DEFAULT_ISR(FMC_IRQHandler)
DEFINE_DEFAULT_ISR(RCU_CTC_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI0_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI1_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI2_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI3_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI4_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel0_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel1_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel2_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel3_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel4_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel5_IRQHandler)
DEFINE_DEFAULT_ISR(DMA0_Channel6_IRQHandler)
DEFINE_DEFAULT_ISR(ADC0_1_IRQHandler)
DEFINE_DEFAULT_ISR(USBD_HP_CAN0_TX_IRQHandler)
DEFINE_DEFAULT_ISR(USBD_LP_CAN0_RX0_IRQHandler)
DEFINE_DEFAULT_ISR(CAN0_RX1_IRQHandler)
DEFINE_DEFAULT_ISR(CAN0_EWMC_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI5_9_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER0_BRK_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER0_UP_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER0_TRG_CMT_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER0_Channel_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER1_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER2_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER3_IRQHandler)
DEFINE_DEFAULT_ISR(I2C0_EV_IRQHandler)
DEFINE_DEFAULT_ISR(I2C0_ER_IRQHandler)
DEFINE_DEFAULT_ISR(I2C1_EV_IRQHandler)
DEFINE_DEFAULT_ISR(I2C1_ER_IRQHandler)
DEFINE_DEFAULT_ISR(SPI0_IRQHandler)
DEFINE_DEFAULT_ISR(SPI1_IRQHandler)
DEFINE_DEFAULT_ISR(USART0_IRQHandler)
DEFINE_DEFAULT_ISR(USART1_IRQHandler)
DEFINE_DEFAULT_ISR(USART2_IRQHandler)
DEFINE_DEFAULT_ISR(EXTI10_15_IRQHandler)
DEFINE_DEFAULT_ISR(RTC_Alarm_IRQHandler)
DEFINE_DEFAULT_ISR(USBD_WKUP_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER7_BRK_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER7_UP_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER7_TRG_CMT_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER7_Channel_IRQHandler)
DEFINE_DEFAULT_ISR(ADC2_IRQHandler)
DEFINE_DEFAULT_ISR(EXMC_IRQHandler)
DEFINE_DEFAULT_ISR(SDIO_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER4_IRQHandler)
DEFINE_DEFAULT_ISR(SPI2_IRQHandler)
DEFINE_DEFAULT_ISR(UART3_IRQHandler)
DEFINE_DEFAULT_ISR(UART4_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER5_IRQHandler)
DEFINE_DEFAULT_ISR(TIMER6_IRQHandler)
DEFINE_DEFAULT_ISR(DMA1_Channel0_IRQHandler)
DEFINE_DEFAULT_ISR(DMA1_Channel1_IRQHandler)
DEFINE_DEFAULT_ISR(DMA1_Channel2_IRQHandler)
DEFINE_DEFAULT_ISR(DMA1_Channel3_4_IRQHandler)

extern uint32_t _estack;

extern "C" void Reset_Handler();

const volatile std::uintptr_t g_pfnVectors[] __attribute__((section(".isr_vector")))
{
    reinterpret_cast<std::uintptr_t>(&_estack),
    reinterpret_cast<std::uintptr_t>(Reset_Handler),
    reinterpret_cast<std::uintptr_t>(NMI_Handler),
    reinterpret_cast<std::uintptr_t>(HardFault_Handler),
    reinterpret_cast<std::uintptr_t>(MemManage_Handler),
    reinterpret_cast<std::uintptr_t>(BusFault_Handler),
    reinterpret_cast<std::uintptr_t>(UsageFault_Handler),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(SVC_Handler),
    reinterpret_cast<std::uintptr_t>(DebugMon_Handler),
    reinterpret_cast<std::uintptr_t>(nullptr),
    reinterpret_cast<std::uintptr_t>(PendSV_Handler),
    reinterpret_cast<std::uintptr_t>(SysTick_Handler),
    reinterpret_cast<std::uintptr_t>(WWDGT_IRQHandler),
    reinterpret_cast<std::uintptr_t>(LVD_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TAMPER_IRQHandler),
    reinterpret_cast<std::uintptr_t>(RTC_IRQHandler),
    reinterpret_cast<std::uintptr_t>(FMC_IRQHandler),
    reinterpret_cast<std::uintptr_t>(RCU_CTC_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI3_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI4_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel3_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel4_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel5_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA0_Channel6_IRQHandler),
    reinterpret_cast<std::uintptr_t>(ADC0_1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USBD_HP_CAN0_TX_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USBD_LP_CAN0_RX0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(CAN0_RX1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(CAN0_EWMC_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI5_9_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER0_BRK_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER0_UP_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER0_TRG_CMT_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER0_Channel_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER3_IRQHandler),
    reinterpret_cast<std::uintptr_t>(I2C0_EV_IRQHandler),
    reinterpret_cast<std::uintptr_t>(I2C0_ER_IRQHandler),
    reinterpret_cast<std::uintptr_t>(I2C1_EV_IRQHandler),
    reinterpret_cast<std::uintptr_t>(I2C1_ER_IRQHandler),
    reinterpret_cast<std::uintptr_t>(SPI0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(SPI1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USART0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USART1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USART2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXTI10_15_IRQHandler),
    reinterpret_cast<std::uintptr_t>(RTC_Alarm_IRQHandler),
    reinterpret_cast<std::uintptr_t>(USBD_WKUP_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER7_BRK_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER7_UP_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER7_TRG_CMT_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER7_Channel_IRQHandler),
    reinterpret_cast<std::uintptr_t>(ADC2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(EXMC_IRQHandler),
    reinterpret_cast<std::uintptr_t>(SDIO_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER4_IRQHandler),
    reinterpret_cast<std::uintptr_t>(SPI2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(UART3_IRQHandler),
    reinterpret_cast<std::uintptr_t>(UART4_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER5_IRQHandler),
    reinterpret_cast<std::uintptr_t>(TIMER6_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA1_Channel0_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA1_Channel1_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA1_Channel2_IRQHandler),
    reinterpret_cast<std::uintptr_t>(DMA1_Channel3_4_IRQHandler),
};
