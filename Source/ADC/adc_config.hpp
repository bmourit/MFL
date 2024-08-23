// gd32f30x ADC peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>
#include "CONFIG.hpp"

namespace adc {


///////////////////////////// BASE ADDRESS /////////////////////////////

enum class ADC_Base {
    ADC0_BASE,
    ADC1_BASE,
    ADC2_BASE,
};

static constexpr unsigned int ADC_baseAddress[] = {
    0x40012400, // ADC0
    0x40012800, // ADC1
    0x40013C00, // ADC2
};


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class ADC_Regs {
    STAT = 0x00,
    CTL0 = 0x04,
    CTL1 = 0x08,
    SAMPT0 = 0x0C,
    SAMPT1 = 0x10,
    IOFF0 = 0x14,
    IOFF1 = 0x18,
    IOFF2 = 0x1C,
    IOFF3 = 0x20,
    WDHT = 0x24,
    WDLT = 0x28,
    RSQ0 = 0x2C,
    RSQ1 = 0x30,
    RSQ2 = 0x34,
    ISQ = 0x38,
    IDATA0 = 0x3C,
    IDATA1 = 0x40,
    IDATA2 = 0x44,
    IDATA3 = 0x48,
    RDATA = 0x4C,
    OVSAMPCTL = 0x80,
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class STAT_Bits {
    WDE = REG_BIT_DEF(0, 0),
    EOC = REG_BIT_DEF(1, 1),
    EOIC = REG_BIT_DEF(2, 2),
    STIC = REG_BIT_DEF(3, 3),
    STRC = REG_BIT_DEF(4, 4),
};

enum class CTL0_Bits {
    WDCHSEL = REG_BIT_DEF(0, 4),
    EOCIE = REG_BIT_DEF(5, 5),
    WDEIE = REG_BIT_DEF(6, 6),
    EOICIE = REG_BIT_DEF(7, 7),
    SM = REG_BIT_DEF(8, 8),
    WDSC = REG_BIT_DEF(9, 9),
    ICA = REG_BIT_DEF(10, 10),
    DISRC = REG_BIT_DEF(11, 11),
    DISIC = REG_BIT_DEF(12, 12),
    DISNUM = REG_BIT_DEF(13, 15),
    SYNCM = REG_BIT_DEF(16, 19),
    IWDEN = REG_BIT_DEF(22, 22),
    RWDEN = REG_BIT_DEF(23, 23),
    DRES = REG_BIT_DEF(24, 25)
};

enum class CTL1_Bits {
    ADCON = REG_BIT_DEF(0, 0),
    CTN = REG_BIT_DEF(1, 1),
    CLB = REG_BIT_DEF(2, 2),
    RSTCLB = REG_BIT_DEF(3, 3),
    DMA = REG_BIT_DEF(8, 8),
    DAL = REG_BIT_DEF(11, 11),
    ETSIC = REG_BIT_DEF(12, 14),
    ETEIC = REG_BIT_DEF(15, 15),
    ETSRC = REG_BIT_DEF(17, 19),
    ETERC = REG_BIT_DEF(20, 20),
    SWICST = REG_BIT_DEF(21, 21),
    SWRCST = REG_BIT_DEF(22, 22),
    TSVREN = REG_BIT_DEF(23, 23),
};

enum class RSQX_Bits {
    RSQN = REG_BIT_DEF(0, 4),
    RL = REG_BIT_DEF(20, 23),
};

enum class ISQ_Bits {
    ISQN = REG_BIT_DEF(0, 4),
    IL = REG_BIT_DEF(20, 21),
};

enum class SAMPTX_Bits {
    SPTN = REG_BIT_DEF(0, 2),
};

enum class IOFFX_Bits {
    IOFF = REG_BIT_DEF(0, 11),
};

enum class WDLT_Bits {
    WDLT = REG_BIT_DEF(0, 11),
};

enum class WDHT_Bits {
    WDHT = REG_BIT_DEF(0, 11),
};

enum class RDATA_Bits {
    RDATA = REG_BIT_DEF(0, 15),
    ADC1RDTR = REG_BIT_DEF(16, 31),
};

enum class OVSAMPCTL_Bits {
    OVSEN = REG_BIT_DEF(0, 0),
    OVSR = REG_BIT_DEF(2, 4),
    OVSS = REG_BIT_DEF(5, 8),
    TOVS = REG_BIT_DEF(9, 9),
    DRES = REG_BIT_DEF(12, 13),
};

enum class ADC_Flags {
    ADC_FLAG_WDE = REG_BIT_DEF(0, 0),
    ADC_FLAG_EOC = REG_BIT_DEF(1, 1),
    EOADC_FLAG_EOICIC = REG_BIT_DEF(2, 2),
    ADC_FLAG_STIC = REG_BIT_DEF(3, 3),
    ADC_FLAG_STRC = REG_BIT_DEF(4, 4),
};


///////////////////////////// ENUMS /////////////////////////////

enum class Sync_Mode {
    SM_FREE,	// independant mode
    SM_RPIP,	// regular parallel + inserted parallel mode
    SM_RPIR,	// regular parallel + trigger rotation mode
    SM_IPRFUF,	// inserted parallel + follow up fast mode
    SM_IPRFUS,	// inserted parallel + follow-up slow mode
    SM_IP,		// inserted parallel mode only
    SM_RP,		// regular parallel mode only
    SM_RFUF,	// regular follow up fast mode only
    SM_RFUS, 	// regular follow up slow mode only
    SM_ITR		// trigger rotation mode only
};

enum class Special_Function {
    SCAN_MODE,
    INSERTED_CH_MODE,
    CONTINUOUS_MODE
};

enum class Data_Alignment {
    DATA_ALIGN_RIGHT,
    DATA_ALIGN_LEFT
};

enum class External_Trigger_Source {
    ADC0_1_REGULAR_TIMER0_CH0 = 0,		// timer 0 CH0 event select
    ADC0_1_REGULAR_TIMER0_CH1 = 1,		// timer 0 CH1 event select
    ADC0_1_REGULAR_TIMER0_CH2 = 2,	 	// timer 0 CH2 event select
    ADC0_1_REGULAR_TIMER1_CH1 = 3, 		// timer 1 CH1 event select
    ADC0_1_REGULAR_TIMER2_TRGO = 4, 	// timer 2 TRGO event select
    ADC0_1_REGULAR_TIMER3_CH3 = 5, 		// timer 3 CH3 event select
    ADC0_1_REGULAR_TIMER7_TRGO = 6,		// timer 7 TRGO event select
    ADC0_1_REGULAR_EXTI_11 = 6, 		// external interrupt line 11
    ADC0_1_REGULAR_NONE = 7, 			// software trigger
    ADC2_REGULAR_TIMER2_CH0 = 0,		// timer 2 CH0 event select
    ADC2_REGULAR_TIMER1_CH2 = 1,		// timer 1 CH2 event select
    ADC2_REGULAR_TIMER0_CH2 = 2,		// timer 0 CH2 event select
    ADC2_REGULAR_TIMER7_CH0 = 3,		// timer 7 CH0 event select
    ADC2_REGULAR_TIMER7_TRGO = 4,		// timer 7 TRGO event select
    ADC2_REGULAR_TIMER4_CH0 = 5,		// timer 4 CH0 event select
    ADC2_REGULAR_TIMER4_CH2 = 6,		// timer 4 CH2 event select
    ADC0_1_INSERTED_TIMER0_TRGO = 0,	// timer 0 TRGO event select
    ADC0_1_INSERTED_TIMER0_CH3 = 1,		// timer 0 CH3 event select
    ADC0_1_INSERTED_TIMER1_TRGO = 2,	// timer 1 TRGO event select
    ADC0_1_INSERTED_TIMER1_CH0 = 3,		// timer 1 CH0 event select
    ADC0_1_INSERTED_TIMER2_CH3 = 4,		// timer 2 CH3 event select
    ADC0_1_INSERTED_TIMER3_TRGO = 5,	// timer 3 TRGO event select
    ADC0_1_INSERTED_EXTI_15 = 6,		// external interrupt line 15
    ADC0_1_INSERTED_TIMER7_CH3 = 6,		// timer 7 CH3 event select
    ADC0_1_2_INSERTED_NONE = 7,			// software trigger
    ADC2_INSERTED_TIMER0_TRGO = 0,		// timer 0 TRGO event select
    ADC2_INSERTED_TIMER0_CH3 = 1,		// timer 0 CH3 event select
    ADC2_INSERTED_TIMER3_CH2 = 2,		// timer 3 CH2 event select
    ADC2_INSERTED_TIMER7_CH1 = 3,		// timer 7 CH1 event select
    ADC2_INSERTED_TIMER7_CH3 = 4,		// timer 7 CH3 event select
    ADC2_INSERTED_TIMER4_TRGO = 5,		// timer 4 TRGO event select
    ADC2_INSERTED_TIMER4_CH3 = 6		// timer 4 CH3 event select
};

enum class ADC_Sample_Time {
    SAMPLETIME_1_5_CYCLES,		// 1.5 cycles
    SAMPLETIME_7_5_CYCLES,		// 7.5 cycles
    SAMPLETIME_13_5_CYCLES,		// 13.5 cycles
    SAMPLETIME_28_5_CYCLES,		// 28.5 cycles
    SAMPLETIME_41_5_CYCLES,		// 41.5 cycles
    SAMPLETIME_55_5_CYCLES,		// 55.5 cycles
    SAMPLETIME_71_5_CYCLES,		// 71.5 cycles
    SAMPLETIME_239_5_CYCLES		// 239.5 cycles
};

enum class ADC_Resolution {
    RESOLUTION_12BIT,
    RESOLUTION_10BIT,
    RESOLUTION_8BIT,
    RESOLUTION_6BIT
};

enum class Oversampling_Shift {
    OVERSAMPLING_SHIFT_NONE,
    OVERSAMPLING_SHIFT_1BIT,
    OVERSAMPLING_SHIFT_2BIT,
    OVERSAMPLING_SHIFT_3BIT,
    OVERSAMPLING_SHIFT_4BIT,
    OVERSAMPLING_SHIFT_5BIT,
    OVERSAMPLING_SHIFT_6BIT,
    OVERSAMPLING_SHIFT_7BIT,
    OVERSAMPLING_SHIFT_8BIT
};

enum class Oversampling_Ratio {
    OVERSAMPLING_RATIO_MUL2,
    OVERSAMPLING_RATIO_MUL4,
    OVERSAMPLING_RATIO_MUL8,
    OVERSAMPLING_RATIO_MUL16,
    OVERSAMPLING_RATIO_MUL32,
    OVERSAMPLING_RATIO_MUL64,
    OVERSAMPLING_RATIO_MUL128,
    OVERSAMPLING_RATIO_MUL256
};

enum class Oversampling_Convertion {
    OVERSAMPLING_CONVERT_ALL,
    OVERSAMPLING_CONVERT_ONE
};

enum class Channel_Group_Type {
    REGULAR_CHANNEL,
    INSERTED_CHANNEL,
    REGULAR_INSERTED_CHANNEL,
    CHANNEL_DISCON_DISABLE,
};

enum class Inserted_Channel {
    INSERTED_CHANNEL_0,
    INSERTED_CHANNEL_1,
    INSERTED_CHANNEL_2,
    INSERTED_CHANNEL_3,
};

enum class ADC_Channel {
    CHANNEL_0,
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
    CHANNEL_5,
    CHANNEL_6,
    CHANNEL_7,
    CHANNEL_8,
    CHANNEL_9,
    CHANNEL_10,
    CHANNEL_11,
    CHANNEL_12,
    CHANNEL_13,
    CHANNEL_14,
    CHANNEL_15,
    CHANNEL_16,
    CHANNEL_17
};

enum class Interrupt_Type {
    INTR_WDE,
    INTR_EOC,
    INTR_EOIC,
};

enum class Interrupt_Flags {
    INTR_FLAG_WDE,
    INTR_FLAG_EOC,
    INTR_FLAG_EOIC,
};

enum class Bit_State {
    BIT_DISABLE,
    BIT_ENABLE
};

enum class ADC_Error_Type {
    OK = 0,
    INVALID_ADC,
    INVALID_OPERATION,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct ADC_Clock_Config {
    rcu::RCU_PCLK clock_reg;
    rcu::RCU_PCLK_Reset reset_reg;
};

static const ADC_Clock_Config ADC_pclk_index[] {
    {rcu::RCU_PCLK::PCLK_ADC0, rcu::RCU_PCLK_Reset::PCLK_ADC0RST},
    {rcu::RCU_PCLK::PCLK_ADC1, rcu::RCU_PCLK_Reset::PCLK_ADC1RST},
    {rcu::RCU_PCLK::PCLK_ADC2, rcu::RCU_PCLK_Reset::PCLK_ADC2RST},
};

}	// namespace adc
