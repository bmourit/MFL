// gd32f30x CORTEX peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "gd32f303re.h"
#include "CORTEX.hpp"

namespace cortex {

// DEPRECATED - use set_low_power_mode_enable
void CORTEX::set_system_low_power_mode(Low_Power_Mode mode)
{
    SCB->SCR |= (uint32_t)(1 << static_cast<uint32_t>(mode));
}

// DEPRECATED - use set_low_power_mode_enable
void CORTEX::reset_system_low_power_mode(Low_Power_Mode mode)
{
    SCB->SCR &= (~(uint32_t)(1 << static_cast<uint32_t>(mode)));
}

// Enable or disable low poer mode
void set_low_power_mode_enable(Low_Power_Mode mode, bool enable)
{
    enable ? (SCB->SCR |= (uint32_t)(1 << static_cast<uint32_t>(mode))) :
    (SCB->SCR &= (~(uint32_t)(1 << static_cast<uint32_t>(mode))));
}

void CORTEX::set_systick_source(Systick_Source source)
{
    if (source == Systick_Source::SYSTICK_SOURCE_HCLK) {
        SysTick->CTRL |= 0xFFFFFFFBU;
    } else {
        SysTick->CTRL &= 0x00000004U;
    }
}

void CORTEX::set_nvic_vector_table(Vector_Table base, uint32_t offset)
{
    SCB->VTOR = static_cast<uint32_t>(NVIC_vectorTable[static_cast<int>(base)]) | (offset & 0x1FFFFF80);
    __DSB();
}

void CORTEX::set_nvic_priority_group(Priority_Group group)
{
    SCB->AIRCR = 0x05FA0000 | static_cast<uint32_t>(NVIC_priorityGroup[static_cast<int>(group)]);
}

void CORTEX::nvic_irq_enable(uint8_t irq, uint8_t preemption_priority, uint8_t sub_priority)
{
    uint32_t t_priority, t_preempt, t_subpriority = 0x00;

    // Use priority group to get preemption and subpriority */
    if (((SCB->AIRCR) & (uint32_t)0x700) == 0x00000700) {
        t_preempt = 0;
        t_subpriority = 0x4;
    } else if (((SCB->AIRCR) & (uint32_t)0x700) == 0x00000600) {
        t_preempt = 1;
        t_subpriority = 0x3;
    } else if (((SCB->AIRCR) & (uint32_t)0x700)== 0x00000500) {
        t_preempt = 2;
        t_subpriority = 0x2;
    } else if (((SCB->AIRCR) & (uint32_t)0x700) == 0x00000400) {
        t_preempt = 3;
        t_subpriority = 0x1;
    } else if (((SCB->AIRCR) & (uint32_t)0x700) == 0x00000300) {
        t_preempt = 4;
        t_subpriority = 0x0;
    } else {
        set_nvic_priority_group(Priority_Group::PRIO_GROUP_PRE2SUB2);
        t_preempt = 2;
        t_subpriority = 0x2;
    }
    // t_priority to set NVIC->IP
    t_priority = (uint32_t)preemption_priority << (0x4 - t_preempt);
    t_priority |= sub_priority & (0x0F >> (0x4 - t_subpriority));
    t_priority = t_priority << 0x04;
    NVIC->IP[irq] = (uint8_t)t_priority;
    // Enable IRQ
    NVIC->ISER[irq >> 0x05] = (uint32_t)0x01 << (irq & (uint8_t)0x1F);
}

void CORTEX::nvic_irq_disable(uint8_t irq)
{
    NVIC->ICER[irq >> 0x05] = (uint32_t)0x01 << (irq & (uint8_t)0x1F);
}

} // namespace cortex

cortex::CORTEX CORTEX_DEVICE;
