// gd32f30x CORTEX peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "gd32f303re.h"
#include "CORTEX.hpp"

namespace cortex {

constexpr uint32_t SysTickSourceHClk = 0xFFFFFFFB;
constexpr uint32_t SysTickMask = 0x00000004;
constexpr uint32_t VectorTableOffsetMask = 0x1FFFFF80;
constexpr uint32_t AIRCRRegisterMask = 0x00000700;

// Enable or disable low power mode
void set_low_power_mode_enable(Low_Power_Mode mode, bool enable)
{
    enable ? (SCB->SCR |= static_cast<uint32_t>(1 << static_cast<uint32_t>(mode))) :
    (SCB->SCR &= (~(static_cast<uint32_t>(1 << static_cast<uint32_t>(mode)))));
}

void CORTEX::set_systick_source(Systick_Source source)
{
    if (source == Systick_Source::SYSTICK_SOURCE_HCLK) {
        SysTick->CTRL |= SysTickSourceHClk;
    } else {
        SysTick->CTRL &= SysTickMask;
    }
}

void CORTEX::set_nvic_vector_table(Vector_Table base, uint32_t offset)
{
    SCB->VTOR = static_cast<uint32_t>(NVIC_vectorTable[static_cast<int>(base)]) | (offset & VectorTableOffsetMask);
    __DSB();
}

void CORTEX::set_nvic_priority_group(Priority_Group group)
{
    SCB->AIRCR = 0x05FA0000 | static_cast<uint32_t>(NVIC_priorityGroup[static_cast<int>(group)]);
}

void CORTEX::nvic_irq_enable(uint8_t irq, uint8_t preemption_priority, uint8_t sub_priority)
{
    uint32_t t_priority, t_preempt, t_subpriority = 0;

    // Use priority group to get preemption and subpriority */
    if (((SCB->AIRCR) & AIRCRRegisterMask) == 0x00000700) {
        t_preempt = 0;
        t_subpriority = 4;
    } else if (((SCB->AIRCR) & AIRCRRegisterMask) == 0x00000600) {
        t_preempt = 1;
        t_subpriority = 3;
    } else if (((SCB->AIRCR) & AIRCRRegisterMask) == 0x00000500) {
        t_preempt = 2;
        t_subpriority = 2;
    } else if (((SCB->AIRCR) & AIRCRRegisterMask) == 0x00000400) {
        t_preempt = 3;
        t_subpriority = 1;
    } else if (((SCB->AIRCR) & AIRCRRegisterMask) == 0x00000300) {
        t_preempt = 4;
        t_subpriority = 0;
    } else {
        set_nvic_priority_group(Priority_Group::PRIO_GROUP_PRE2SUB2);
        t_preempt = 2;
        t_subpriority = 2;
    }
    // t_priority to set NVIC->IP
    t_priority = static_cast<uint32_t>(preemption_priority << (4 - t_preempt));
    t_priority |= static_cast<uint32_t>(sub_priority & (0xF >> (4 - t_subpriority)));
    t_priority = t_priority << 4;
    NVIC->IP[irq] = static_cast<uint8_t>(t_priority);
    // Enable IRQ
    NVIC->ISER[irq >> 5] = static_cast<uint32_t>(1 << (irq & 0x1F));
}

void CORTEX::nvic_irq_disable(uint8_t irq)
{
    NVIC->ICER[irq >> 5] = static_cast<uint32_t>(1 << (irq & 0x1F));
}

} // namespace cortex

cortex::CORTEX CORTEX_DEVICE;
