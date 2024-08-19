// CMSIS compatability

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////// NOTICE ////////////////////////////////////////
//  Some early batches of this chip shipped without an FPU/MPU.
//	If you experience issues, you may need to disable these features.
////////////////////////////////////////////////////////////////////////////////////////

// Cortex-M4 processor and peripherals configuration
#define __CM4_REV                 0x0001	// Core revision r0p1
#define __MPU_PRESENT             0			// GD32F30x provide MPU
#define __NVIC_PRIO_BITS          4			// GD32F30x uses 4 bits for the priority levels
#define __Vendor_SysTickConfig    0			// set to 1 if different SysTick config is used
#define __FPU_PRESENT			  1

// IRQs
typedef enum IRQn {
    // Cortex-M4
    NonMaskableInt_IRQn          = -14,
    HardFault_IRQn               = -13,
    MemoryManagement_IRQn        = -12,
    BusFault_IRQn                = -11,
    UsageFault_IRQn              = -10,
    SVCall_IRQn                  = -5,
    DebugMonitor_IRQn            = -4,
    PendSV_IRQn                  = -2,
    SysTick_IRQn                 = -1,
    // GD32F303RE
    WWDGT_IRQn                   = 0,
    LVD_IRQn                     = 1,
    TAMPER_IRQn                  = 2,
    RTC_IRQn                     = 3,
    FMC_IRQn                     = 4,
    RCU_CTC_IRQn                 = 5,
    EXTI0_IRQn                   = 6,
    EXTI1_IRQn                   = 7,
    EXTI2_IRQn                   = 8,
    EXTI3_IRQn                   = 9,
    EXTI4_IRQn                   = 10,
    DMA0_Channel0_IRQn           = 11,
    DMA0_Channel1_IRQn           = 12,
    DMA0_Channel2_IRQn           = 13,
    DMA0_Channel3_IRQn           = 14,
    DMA0_Channel4_IRQn           = 15,
    DMA0_Channel5_IRQn           = 16,
    DMA0_Channel6_IRQn           = 17,
    ADC0_1_IRQn                  = 18,
    USBD_HP_CAN0_TX_IRQn         = 19,
    USBD_LP_CAN0_RX0_IRQn        = 20,
    CAN0_RX1_IRQn                = 21,
    CAN0_EWMC_IRQn               = 22,
    EXTI5_9_IRQn                 = 23,
    TIMER0_BRK_IRQn              = 24,
    TIMER0_UP_IRQn               = 25,
    TIMER0_TRG_CMT_IRQn          = 26,
    TIMER0_Channel_IRQn          = 27,
    TIMER1_IRQn                  = 28,
    TIMER2_IRQn                  = 29,
    TIMER3_IRQn                  = 30,
    I2C0_EV_IRQn                 = 31,
    I2C0_ER_IRQn                 = 32,
    I2C1_EV_IRQn                 = 33,
    I2C1_ER_IRQn                 = 34,
    SPI0_IRQn                    = 35,
    SPI1_IRQn                    = 36,
    USART0_IRQn                  = 37,
    USART1_IRQn                  = 38,
    USART2_IRQn                  = 39,
    EXTI10_15_IRQn               = 40,
    RTC_Alarm_IRQn               = 41,
    USBD_WKUP_IRQn               = 42,
    TIMER7_BRK_IRQn              = 43,
    TIMER7_UP_IRQn               = 44,
    TIMER7_TRG_CMT_IRQn          = 45,
    TIMER7_Channel_IRQn          = 46,
    ADC2_IRQn                    = 47,
    EXMC_IRQn                    = 48,
    SDIO_IRQn                    = 49,
    TIMER4_IRQn                  = 50,
    SPI2_IRQn                    = 51,
    UART3_IRQn                   = 52,
    UART4_IRQn                   = 53,
    TIMER5_IRQn                  = 54,
    TIMER6_IRQn                  = 55,
    DMA1_Channel0_IRQn           = 56,
    DMA1_Channel1_IRQn           = 57,
    DMA1_Channel2_IRQn           = 58,
    DMA1_Channel3_Channel4_IRQn  = 59
} IRQn_Type;

#include "core_cm4.h"
#include <stdint.h>

#ifdef __cplusplus
}
#endif
