// gd32f30x EXTI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#include "EXTI.hpp"

namespace exti {

void EXTI::init(EXTI_Line line, EXTI_Mode mode, EXTI_Trigger trigger)
{
    // Reset
    write_bit(*this, EXTI_Regs::INTEN, static_cast<uint32_t>(line), Clear);
    write_bit(*this, EXTI_Regs::EVEN, static_cast<uint32_t>(line), Clear);
    write_bit(*this, EXTI_Regs::RTEN, static_cast<uint32_t>(line), Clear);
    write_bit(*this, EXTI_Regs::FTEN, static_cast<uint32_t>(line), Clear);

    // EXTI mode with interrupts or events
    switch (mode) {
    case EXTI_Mode::EXTI_INTERRUPT:
        write_bit(*this, EXTI_Regs::INTEN, static_cast<uint32_t>(line), Set);
        break;
    case EXTI_Mode::EXTI_EVENT:
        write_bit(*this, EXTI_Regs::EVEN, static_cast<uint32_t>(line), Set);
        break;
    default:
        break;
    }

    // EXTI trigger
    switch (trigger) {
    case EXTI_Trigger::TRIG_RISING:
        write_bit(*this, EXTI_Regs::RTEN, static_cast<uint32_t>(line), Set);
        write_bit(*this, EXTI_Regs::RTEN, static_cast<uint32_t>(line), Clear);
        break;
    case EXTI_Trigger::TRIG_FALLING:
        write_bit(*this, EXTI_Regs::RTEN, static_cast<uint32_t>(line), Clear);
        write_bit(*this, EXTI_Regs::FTEN, static_cast<uint32_t>(line), Set);
        break;
    case EXTI_Trigger::TRIG_BOTH:
        write_bit(*this, EXTI_Regs::RTEN, static_cast<uint32_t>(line), Set);
        write_bit(*this, EXTI_Regs::FTEN, static_cast<uint32_t>(line), Set);
        break;
    case EXTI_Trigger::TRIG_NONE:
    default:
        break;
    }
}

void EXTI::reset(void)
{
    // Reset all
    write_register(EXTI_Regs::INTEN, Clear);
    write_register(EXTI_Regs::EVEN, Clear);
    write_register(EXTI_Regs::RTEN, Clear);
    write_register(EXTI_Regs::FTEN, Clear);
    write_register(EXTI_Regs::SWIEV, Clear);
}

bool EXTI::get_flag(EXTI_Line line)
{
    return (read_bit(*this, EXTI_Regs::PD, static_cast<uint32_t>(line)) != Clear) ? true : false;
}

void EXTI::clear_flag(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::PD, static_cast<uint32_t>(line), Set);
}

bool EXTI::get_interrupt_flag(EXTI_Line line)
{
    return (read_bit(*this, EXTI_Regs::PD, static_cast<uint32_t>(line)) != Clear) ? true : false;
}

void EXTI::clear_interrupt_flag(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::PD, static_cast<uint32_t>(line), Set);
}

// DEPRECATED - use set_event_enable
void EXTI::event_enable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::EVEN, static_cast<uint32_t>(line), Set);
}

// DEPRECATED - use set_event_enable
void EXTI::event_disable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::EVEN, static_cast<uint32_t>(line), Clear);
}

// Enable or disable event
void EXTI::set_event_enable(EXTI_Line line, bool enable)
{
    write_bit(*this, EXTI_Regs::EVEN, static_cast<uint32_t>(line), enable ? Set : Clear);
}

// DEPRECATED - use set_software_interrrupt_enable
void EXTI::software_interrupt_enable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::SWIEV, static_cast<uint32_t>(line), Set);
}

// DEPRECATED - use set_software_interrrupt_enable
void EXTI::software_interrupt_disable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::SWIEV, static_cast<uint32_t>(line), Clear);
}

// Enable or disable software interrupt
void EXTI::set_software_interrupt_enable(EXTI_Line line, bool enable)
{
    write_bit(*this, EXTI_Regs::SWIEV, static_cast<uint32_t>(line), enable ? Set : Clear);
}

// DEPRECATED - use set_interrupt_enable
void EXTI::interrupt_enable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::INTEN, static_cast<uint32_t>(line), Set);
}

// DEPRECATED - use set_interrupt_enable
void EXTI::interrupt_disable(EXTI_Line line)
{
    write_bit(*this, EXTI_Regs::INTEN, static_cast<uint32_t>(line), Clear);
}

// Enable or disable interrupt
void EXTI::set_interrupt_enable(EXTI_Line line, bool enable)
{
    write_bit(*this, EXTI_Regs::INTEN, static_cast<uint32_t>(line), enable ? Set : Clear);
}

} // namespace exti

exti::EXTI EXTI_DEVICE;
