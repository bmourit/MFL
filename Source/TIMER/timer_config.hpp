// gd32f30x TIMER peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "GPIO.hpp"
#include "CONFIG.hpp"

namespace timer {

///////////////////////////// BASE ADDRESS /////////////////////////////

enum class TIMER_Base {
    TIMER0_BASE,
    TIMER1_BASE,
    TIMER2_BASE,
    TIMER3_BASE,
    TIMER4_BASE,
    TIMER5_BASE,
    TIMER6_BASE,
    TIMER7_BASE,
};

static constexpr uintptr_t TIMER_baseAddress[] = {
    0x40012C00, // TIMER0
    0x40000000, // TIMER1
    0x40000400, // TIMER2
    0x40000800, // TIMER3
    0x40000C00, // TIMER4
    0x40001000, // TIMER5
    0x40001400, // TIMER6
    0x40013400, // TIMER7
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class TIMER_Regs {
    CTL0 = 0x00,
    CTL1 = 0x04,
    SMCFG = 0x08,
    DMAINTEN = 0x0C,
    INTF = 0x10,
    SWEVG = 0x14,
    CHCTL0 = 0x18,
    CHCTL1 = 0x1C,
    CHCTL2 = 0x20,
    CNT = 0x24,
    PSC = 0x28,
    CAR = 0x2C,
    CREP = 0x30,
    CH0CV = 0x34,
    CH1CV = 0x38,
    CH2CV = 0x3C,
    CH3CV = 0x40,
    CCHP = 0x44,
    DMACFG = 0x48,
    DMATB = 0x4C,
    IRMP = 0x50,
    CFG = 0xFC,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class CTL0_Bits {
    CEN = REG_BIT_DEF(0, 0),
    UPDIS = REG_BIT_DEF(1, 1),
    UPS = REG_BIT_DEF(2, 2),
    SPM = REG_BIT_DEF(3, 3),
    DIR = REG_BIT_DEF(4, 4),
    CAM = REG_BIT_DEF(5, 6),
    ARSE = REG_BIT_DEF(7, 7),
    CKDIV = REG_BIT_DEF(8, 9),
};

enum class CTL1_Bits {
    CCSE = REG_BIT_DEF(0, 0),
    CCUC = REG_BIT_DEF(2, 2),
    DMAS = REG_BIT_DEF(3, 3),
    MMC = REG_BIT_DEF(4, 6),
    TI0S = REG_BIT_DEF(7, 7),
    ISO0 = REG_BIT_DEF(8, 8),
    ISO0N = REG_BIT_DEF(9, 9),
    ISO1 = REG_BIT_DEF(10, 10),
    ISO1N = REG_BIT_DEF(11, 11),
    ISO2 = REG_BIT_DEF(12, 12),
    ISO2N = REG_BIT_DEF(13, 13),
    ISO3 = REG_BIT_DEF(14, 14),
};

enum class SMCFG_Bits {
    SMC = REG_BIT_DEF(0, 2),
    TRGS = REG_BIT_DEF(4, 6),
    MSM = REG_BIT_DEF(7, 7),
    ETFC = REG_BIT_DEF(8, 11),
    ETPSC = REG_BIT_DEF(12, 13),
    SMC1 = REG_BIT_DEF(14, 14),
    ETP = REG_BIT_DEF(15, 15),
};

enum class DMAINTEN_Bits {
    UPIE = REG_BIT_DEF(0, 0),
    CH0IE = REG_BIT_DEF(1, 1),
    CH1IE = REG_BIT_DEF(2, 2),
    CH2IE = REG_BIT_DEF(3, 3),
    CH3IE = REG_BIT_DEF(4, 4),
    CMTIE = REG_BIT_DEF(5, 5),
    TRGIE = REG_BIT_DEF(6, 6),
    BRKIE = REG_BIT_DEF(7, 7),
    UPDEN = REG_BIT_DEF(8, 8),
    CH0DEN = REG_BIT_DEF(9, 9),
    CH1DEN = REG_BIT_DEF(10, 10),
    CH2DEN = REG_BIT_DEF(11, 11),
    CH3DEN = REG_BIT_DEF(12, 12),
    CMTDEN = REG_BIT_DEF(13, 13),
    TRGDEN = REG_BIT_DEF(14, 14),
};

enum class INTF_Bits {
    UPIF = REG_BIT_DEF(0, 0),
    CH0IF = REG_BIT_DEF(1, 1),
    CH1IF = REG_BIT_DEF(2, 2),
    CH2IF = REG_BIT_DEF(3, 3),
    CH3IF = REG_BIT_DEF(4, 4),
    CMTIF = REG_BIT_DEF(5, 5),
    TRGIF = REG_BIT_DEF(6, 6),
    BRKIF = REG_BIT_DEF(7, 7),
    CH0OF = REG_BIT_DEF(9, 9),
    CH1OF = REG_BIT_DEF(10, 10),
    CH2OF = REG_BIT_DEF(11, 11),
    CH3OF = REG_BIT_DEF(12, 12),
};

enum class SWEVG_Bits {
    UPG = REG_BIT_DEF(0, 0),
    CH0G = REG_BIT_DEF(1, 1),
    CH1G = REG_BIT_DEF(2, 2),
    CH2G = REG_BIT_DEF(3, 3),
    CH3G = REG_BIT_DEF(4, 4),
    CMTG = REG_BIT_DEF(5, 5),
    TRGG = REG_BIT_DEF(6, 6),
    BRKG = REG_BIT_DEF(7, 7),
};

enum class CHCTL0_Bits {
    // Output compare mode
    CH0MS = REG_BIT_DEF(0, 1),
    CH0COMFEN = REG_BIT_DEF(2, 2),
    CH0COMSEN = REG_BIT_DEF(3, 3),
    CH0COMCTL = REG_BIT_DEF(4, 6),
    CH0COMCEN = REG_BIT_DEF(7, 7),
    CH1MS = REG_BIT_DEF(8, 9),
    CH1COMFEN = REG_BIT_DEF(10, 10),
    CH1COMSEN = REG_BIT_DEF(11, 11),
    CH1COMCTL = REG_BIT_DEF(12, 14),
    CH1COMCEN = REG_BIT_DEF(15, 15),
    // Input capture mode
    CH0CAPPSC = REG_BIT_DEF(2, 3),
    CH0CAPFLT = REG_BIT_DEF(4, 7),
    CH1CAPPSC = REG_BIT_DEF(10, 11),
    CH1CAPFLT = REG_BIT_DEF(12, 15),
};

enum class CHCTL1_Bits {
    // Output compare mode
    CH2MS = REG_BIT_DEF(0, 1),
    CH2COMFEN = REG_BIT_DEF(2, 2),
    CH2COMSEN = REG_BIT_DEF(3, 3),
    CH2COMCTL = REG_BIT_DEF(4, 6),
    CH2COMCEN = REG_BIT_DEF(7, 7),
    CH3MS = REG_BIT_DEF(8, 9),
    CH3COMFEN = REG_BIT_DEF(10, 10),
    CH3COMSEN = REG_BIT_DEF(11, 11),
    CH3COMCTL = REG_BIT_DEF(12, 14),
    CH3COMCEN = REG_BIT_DEF(15, 15),
    // Input campture mode
    CH2CAPPSC = REG_BIT_DEF(2, 3),
    CH2CAPFLT = REG_BIT_DEF(4, 7),
    CH3CAPPSC = REG_BIT_DEF(10, 11),
    CH3CAPFLT = REG_BIT_DEF(12, 15),
};

enum class CHCTL2_Bits {
    CH0EN = REG_BIT_DEF(0, 0),
    CH0P = REG_BIT_DEF(1, 1),
    CH0NEN = REG_BIT_DEF(2, 2),
    CH0NP = REG_BIT_DEF(3, 3),
    CH1EN = REG_BIT_DEF(4, 4),
    CH1P = REG_BIT_DEF(5, 5),
    CH1NEN = REG_BIT_DEF(6, 6),
    CH1NP = REG_BIT_DEF(7, 7),
    CH2EN = REG_BIT_DEF(8, 8),
    CH2P = REG_BIT_DEF(9, 9),
    CH2NEN = REG_BIT_DEF(10, 10),
    CH2NP = REG_BIT_DEF(11, 11),
    CH3EN = REG_BIT_DEF(12, 12),
    CH3P = REG_BIT_DEF(13, 13),
};

enum class CNT_Bits {
    CNT = REG_BIT_DEF(0, 15),
};

enum class PSC_Bits {
    PSC = REG_BIT_DEF(0, 15),
};

enum class CAR_Bits {
    CARL = REG_BIT_DEF(0, 15),
};

enum class CREP_Bits {
    CREP = REG_BIT_DEF(0, 7),
};

enum class CHxCV_Bits {
    CHxCV = REG_BIT_DEF(0, 15),
};

enum class CCHP_Bits {
    DTCFG = REG_BIT_DEF(0, 7),
    PROT = REG_BIT_DEF(8, 9),
    IOS = REG_BIT_DEF(10, 10),
    ROS = REG_BIT_DEF(11, 11),
    BRKEN = REG_BIT_DEF(12, 12),
    BRKP = REG_BIT_DEF(13, 13),
    OAEN = REG_BIT_DEF(14, 14),
    POEN = REG_BIT_DEF(15, 15),
};

enum class DMACFG_Bits {
    DMATA = REG_BIT_DEF(0, 4),
    DMATC = REG_BIT_DEF(8, 12),
};

enum class DMATB_Bits {
    DMATB = REG_BIT_DEF(0, 15),
};

enum class CFG_Bits {
    OUTSEL = REG_BIT_DEF(0, 0),
    CHVSEL = REG_BIT_DEF(1, 1),
};

///////////////////////////// ENUMS /////////////////////////////

enum class Status_Flags {
    FLAG_UPIF,
    FLAG_CH0,
    FLAG_CH1,
    FLAG_CH2,
    FLAG_CH3,
    FLAG_CMT,
    FLAG_TRG,
    FLAG_BRK,
    FLAG_CH0O,
    FLAG_CH1O,
    FLAG_CH2O,
    FLAG_CH3O,
};

enum class Interrupt_Flags {
    INTR_FLAG_UP,
    INTR_FLAG_CH0,
    INTR_FLAG_CH1,
    INTR_FLAG_CH2,
    INTR_FLAG_CH3,
    INTR_FLAG_CMT,
    INTR_FLAG_TRG,
    INTR_FLAG_BRK,
};

enum class Interrupt_Type {
    INTR_UPIE,
    INTR_CH0IE,
    INTR_CH1IE,
    INTR_CH2IE,
    INTR_CH3IE,
    INTR_CMTIE,
    INTR_TRGIE,
    INTR_BRKIE,
};

enum class DMA_Request {
    UPDATE_EVENT,
    CHANNEL_EVENT,
};

// DMA transfer access
enum class DMA_Transfer_Address {
    DMATA_CTL0,
    DMATA_CTL1,
    DMATA_SMCFG,
    DMATA_DMAINTEN,
    DMATA_INTF,
    DMATA_SWEVG,
    DMATA_CHCTL0,
    DMATA_CHCTL1,
    DMATA_CHCTL2,
    DMATA_CNT,
    DMATA_PSC,
    DMATA_CAR,
    DMATA_CREP,
    DMATA_CH0CV,
    DMATA_CH1CV,
    DMATA_CH2CV,
    DMATA_CH3CV,
    DMATA_CCHP,
    DMATA_DMACFG,
    DMATA_DMATB,
};

// DMA transfer count (burst length)
enum class DMA_Burst_Length {
    DMATC_TRANSFER_COUNT1,
    DMATC_TRANSFER_COUNT2,
    DMATC_TRANSFER_COUNT3,
    DMATC_TRANSFER_COUNT4,
    DMATC_TRANSFER_COUNT5,
    DMATC_TRANSFER_COUNT6,
    DMATC_TRANSFER_COUNT7,
    DMATC_TRANSFER_COUNT8,
    DMATC_TRANSFER_COUNT9,
    DMATC_TRANSFER_COUNT10,
    DMATC_TRANSFER_COUNT11,
    DMATC_TRANSFER_COUNT12,
    DMATC_TRANSFER_COUNT13,
    DMATC_TRANSFER_COUNT14,
    DMATC_TRANSFER_COUNT15,
    DMATC_TRANSFER_COUNT16,
    DMATC_TRANSFER_COUNT17,
    DMATC_TRANSFER_COUNT18,
};

enum class Event_Source {
    EVENT_SRC_UPG,
    EVENT_SRC_CH0G,
    EVENT_SRC_CH1G,
    EVENT_SRC_CH2G,
    EVENT_SRC_CH3G,
    EVENT_SRC_CMTG,
    EVENT_SRC_TRGG,
    EVENT_SRC_BRKG,
};

// Center aligned mode
enum class Center_Align {
    EDGE,
    CENTER_DOWN,
    CENTER_UP,
    CENTER_BOTH,
};

enum class PSC_Reload {
    RELOAD_NOW,
    RELOAD_UPDATE,
};

enum class Count_Direction {
    UP,
    DOWN,
};

enum class Division_Ratio {
    DIV1,
    DIV2,
    DIV4,
};

// Single pulse mode
enum class Pulse_Mode {
    SINGLE_PULSE,
    REPEAT_PULSE,
};

enum class Update_Source {
    REGULAR_SOURCE,
    GLOBAL_SOURCE,
};

// Run off-state mode
enum class ROS_State {
    ROS_DISABLE,
    ROS_ENABLE,
};

// Idle off-state mode
enum class IOS_State {
    IOS_DISABLE,
    IOS_ENABLE,
};

// Break input polarity
enum class Break_Polarity {
    BREAK_LOW,
    BREAK_HIGH,
};

// Output auto
enum class Output_Auto {
    OUTPUT_AUTO_DISABLE,
    OUTPUT_AUTO_ENABLE,
};

// Companion channel protect
enum class CCHP_Protect {
    CCHP_OFF,
    CCHP_PROT0,
    CCHP_PROT1,
    CCHP_PROT2,
};

// Break input
enum class Break_Input {
    BREAK_DISABLE,
    BREAK_ENABLE,
};

enum class Timer_Channel {
    CH0,
    CH1,
    CH2,
    CH3,
};

// Channel state
enum class Output_Compare_State {
    OUTPUT_COMPARE_DISABLE,
    OUTPUT_COMPARE_ENABLE,
};

// Generic polarity
enum class Polarity_Select {
    HIGH_RISING,
    LOW_FALLING,
};

enum class Idle_State {
    IDLE_LOW,
    IDLE_HIGH,
};

enum class Output_Compare_Mode {
    OC_TIMING_MODE,
    OC_OUTPUT_MODE,
    OC_CLEAR_MODE,
    OC_TOGGLE_MODE,
    OC_FORCE_LOW_MODE,
    OC_FORCE_HIGH_MODE,
    OC_PWM_MODE0,
    OC_PWM_MODE1,
};

enum class Output_Compare_Shadow {
    OC_SHADOW_DISABLE,
    OC_SHADOW_ENABLE,
};

enum class Output_Compare_Fast {
    OC_FAST_DISABLE,
    OC_FAST_ENABLE,
};

enum class Output_Compare_Clear {
    OC_CLEAR_DISABLE,
    OC_CLEAR_ENABLE,
};

// Channel control shadow update
enum class Shadow_Update {
    SHADOW_CCU,
    SHADOW_CCUTRI,
};

//
// Input_Capture_Select
//
// For CH0MS:
//      Generate trigger from input source connected to CI0FE0
// For CH0MS:
//      Generate trigger from input source connected to CH1FE0
// ITS:
//      Generatte trigger from selected internal trigger source
//
enum class Input_Capture_Select {
    IO_OUTPUT,
    IO_INPUT_CI0FE0,
    IO_INPUT_CI1FE0,
    IO_INPUT_ITS,
};

enum class Input_Capture_Prescaler {
    DIV1,
    DIV2,
    DIV4,
    DIV8,
};

enum class Trigger_Select {
    ITI0,
    ITI1,
    ITI2,
    ITI3,
    CI0F_ED,
    CI0FE0,
    CI1FE1,
    ETIFP,
};

enum class Master_Control {
    RESET,
    ENABLE,
    UPDATE,
    CH0,
    O0CPRE,
    O1CPRE,
    O2CPRE,
    O3CPRE,
};

enum class Slave_Control {
    DISABLE = 0,
    RESTART = 4,
    PAUSE = 5,
    EVENT = 6,
    EXTERNAL0 = 7,
};

enum class Decode_Mode {
    ENCODER0 = 1,
    ENCODER1 = 2,
    ENCODER2 = 3,
};

enum class External_Trigger_Prescaler {
    OFF,
    DIV2,
    DIV4,
    DIV8,
};

enum class Hall_Interface {
    HALL_ENABLE,
    HALL_DISABLE,
};

enum class TIMER_Error_Type {
    OK = 0,
    INVALID_TIMER,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct TIMER_Clock_Config {
    rcu::RCU_PCLK clock_reg;
    rcu::RCU_PCLK_Reset reset_reg;
};

static const TIMER_Clock_Config TIMER_pclk_index[] {
    {rcu::RCU_PCLK::PCLK_TIMER0, rcu::RCU_PCLK_Reset::PCLK_TIMER0RST},
    {rcu::RCU_PCLK::PCLK_TIMER1, rcu::RCU_PCLK_Reset::PCLK_TIMER1RST},
    {rcu::RCU_PCLK::PCLK_TIMER2, rcu::RCU_PCLK_Reset::PCLK_TIMER2RST},
    {rcu::RCU_PCLK::PCLK_TIMER3, rcu::RCU_PCLK_Reset::PCLK_TIMER3RST},
    {rcu::RCU_PCLK::PCLK_TIMER4, rcu::RCU_PCLK_Reset::PCLK_TIMER4RST},
    {rcu::RCU_PCLK::PCLK_TIMER5, rcu::RCU_PCLK_Reset::PCLK_TIMER5RST},
    {rcu::RCU_PCLK::PCLK_TIMER6, rcu::RCU_PCLK_Reset::PCLK_TIMER6RST},
    {rcu::RCU_PCLK::PCLK_TIMER7, rcu::RCU_PCLK_Reset::PCLK_TIMER7RST},
};

struct TIMER_Pin_Config {
    gpio::GPIO_Base gpio_port;
    gpio::Pin_Number pin;
    gpio::Pin_Mode mode;
    gpio::Output_Speed speed;
};

struct TIMER_Break {
    Break_Input break_state;
    Break_Polarity break_polarity;
    Output_Auto output_auto_state;
    ROS_State ros_state;
    IOS_State ios_state;
    CCHP_Protect protection;
    uint16_t dead_time;
};

struct TIMER_Input_Capture {
    Polarity_Select polarity;
    Input_Capture_Select source_select;
    Input_Capture_Prescaler prescaler;
    uint16_t digital_filter;
};

struct TIMER_Output_Compare {
    Output_Compare_State state;
    Output_Compare_State companion_state;
    Polarity_Select polarity;
    Polarity_Select companion_polarity;
    Idle_State idle_state;
    Idle_State companion_idle_state;
};

struct TIMER_Config {
    uint32_t prescaler;
    uint32_t period;
    Division_Ratio divider;
    Center_Align align;
    Count_Direction counting_direction;
    uint32_t repetition_count;
};

} // namespace timer
