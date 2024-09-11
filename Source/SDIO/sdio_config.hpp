// gd32f30x SDIO peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace sdio {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class SDIO_Regs {
    PWRCTL = 0x00,
    CLKCTL = 0x04,
    CMDAGMT = 0x08,
    CMDCTL	 = 0x0C,
    RSPCMDIDX = 0x10,
    RESP0 = 0x14,
    RESP1 = 0x18,
    RESP2 = 0x1C,
    RESP3 = 0x20,
    DATATO = 0x24,
    DATALEN = 0x28,
    DATACTL = 0x2C,
    DATACNT = 0x30,
    STAT = 0x34,
    INTC = 0x38,
    INTEN = 0x3C,
    FIFOCNT = 0x48,
    FIFO = 0x80,
};


///////////////////////////// REGISTER BITS /////////////////////////////


enum class PWRCTL_Bits {
    PWRCTL = REG_BIT_DEF(0, 1),
};

enum class CLKCTL_Bits {
    DIV = REG_BIT_DEF(0, 7),
    CLKEN = REG_BIT_DEF(8, 8),
    CLKPWRSAV = REG_BIT_DEF(9, 9),
    CLKBYP = REG_BIT_DEF(10, 10),
    BUSMODE	= REG_BIT_DEF(11, 12),
    CLKEDGE = REG_BIT_DEF(13, 13),
    HWCLKEN = REG_BIT_DEF(14, 14),
    DIV8 = REG_BIT_DEF(31, 31),
};

enum class CMDAGMT_Bits {
    CMDAGMT = REG_BIT_DEF(0, 31),
};

enum class CMDCTL_Bits {
    CMDIDX = REG_BIT_DEF(0, 5),
    CMDRESP = REG_BIT_DEF(6, 7),
    INTWAIT = REG_BIT_DEF(8, 8),
    WAITDEND = REG_BIT_DEF(9, 9),
    WAITTYPE = REG_BIT_DEF(8, 9),
    CSMEN = REG_BIT_DEF(10, 10),
    SUSPEND = REG_BIT_DEF(11, 12),
    ENCMDC = REG_BIT_DEF(12, 12),
    NINTEN = REG_BIT_DEF(13, 13),
    ATAEN = REG_BIT_DEF(14, 14),
};

enum class RSPCMDIDX_Bits {
    RSPCMDIDX = REG_BIT_DEF(0, 5),
};

enum class DATALEN_Bits {
    DATALEN = REG_BIT_DEF(0, 24),
};

enum class DATACTL_Bits {
    DATAEN = REG_BIT_DEF(0, 0),
    DATADIR = REG_BIT_DEF(1, 1),
    TRANSMOD = REG_BIT_DEF(2, 2),
    DMAEN = REG_BIT_DEF(3, 3),
    BLKSZ = REG_BIT_DEF(4, 7),
    RWEN = REG_BIT_DEF(8, 8),
    RWSTOP = REG_BIT_DEF(9, 9),
    RWTYPE = REG_BIT_DEF(10, 10),
    IOEN = REG_BIT_DEF(11, 11),
};

enum class DATACNT_Bits {
    DATACNT = REG_BIT_DEF(0, 24),
};

enum class STAT_Bits {
    CCRCERR = REG_BIT_DEF(0, 0),
    DTCRCERR = REG_BIT_DEF(1, 1),
    CMDTMOUT = REG_BIT_DEF(2, 2),
    DTTMOUT = REG_BIT_DEF(3, 3),
    TXURE = REG_BIT_DEF(4, 4),
    RXORE = REG_BIT_DEF(5, 5),
    CMDRECV = REG_BIT_DEF(6, 6),
    CMDSEND = REG_BIT_DEF(7, 7),
    DTEND = REG_BIT_DEF(8, 8),
    STBITE = REG_BIT_DEF(9, 9),
    DTBLKEND = REG_BIT_DEF(10, 10),
    CMDRUN = REG_BIT_DEF(11, 11),
    TXRUN = REG_BIT_DEF(12, 12),
    RXRUN = REG_BIT_DEF(13, 13),
    TFH = REG_BIT_DEF(14, 14),
    RFH = REG_BIT_DEF(15, 15),
    TFF = REG_BIT_DEF(16, 16),
    RFF = REG_BIT_DEF(17, 17),
    TFE = REG_BIT_DEF(18, 18),
    RFE = REG_BIT_DEF(19, 19),
    TXDTVAL = REG_BIT_DEF(20, 20),
    RXDTVAL = REG_BIT_DEF(21, 21),
    SDIOINT = REG_BIT_DEF(22, 22),
    ATAEND = REG_BIT_DEF(23, 23),
};

enum class INTC_Bits {
    CCRCERRC = REG_BIT_DEF(0, 0),
    DTCRCERRC = REG_BIT_DEF(1, 1),
    CMDTMOUTC = REG_BIT_DEF(2, 2),
    DTTMOUTC = REG_BIT_DEF(3, 3),
    TXUREC = REG_BIT_DEF(4, 4),
    RXOREC = REG_BIT_DEF(5, 5),
    CMDRECVC = REG_BIT_DEF(6, 6),
    CMDSENDC = REG_BIT_DEF(7, 7),
    DTENDC = REG_BIT_DEF(8, 8),
    STBITEC = REG_BIT_DEF(9, 9),
    DTBLKENDC = REG_BIT_DEF(10, 10),
    SDIOINTC = REG_BIT_DEF(22, 22),
    ATAENDC = REG_BIT_DEF(23, 23),
};

enum class INTEN_Bits {
    CCRCERRIE = REG_BIT_DEF(0, 0),
    DTCRCERRIE = REG_BIT_DEF(1, 1),
    CMDTMOUTIE = REG_BIT_DEF(2, 2),
    DTTMOUTIE = REG_BIT_DEF(3, 3),
    TXUREIE = REG_BIT_DEF(4, 4),
    RXOREIE = REG_BIT_DEF(5, 5),
    CMDRECVIE = REG_BIT_DEF(6, 6),
    CMDSENDIE = REG_BIT_DEF(7, 7),
    DTENDIE = REG_BIT_DEF(8, 8),
    STBITEIE = REG_BIT_DEF(9, 9),
    DTBLKENDIE = REG_BIT_DEF(10, 10),
    CMDRUNIE = REG_BIT_DEF(11, 11),
    TXRUNIE = REG_BIT_DEF(12, 12),
    RXRUNIE = REG_BIT_DEF(13, 13),
    TFHIE = REG_BIT_DEF(14, 14),
    RFHIE = REG_BIT_DEF(15, 15),
    TFFIE = REG_BIT_DEF(16, 16),
    RFFIE = REG_BIT_DEF(17, 17),
    TFEIE = REG_BIT_DEF(18, 18),
    RFEIE = REG_BIT_DEF(19, 19),
    TXDTVALIE = REG_BIT_DEF(20, 20),
    RXDTVALIE = REG_BIT_DEF(21, 21),
    SDIOINTIE = REG_BIT_DEF(22, 22),
    ATAENDIE = REG_BIT_DEF(23, 23),
};

enum class FIFOCNT_Bits {
    FIFOCNT = REG_BIT_DEF(0, 23),
};

enum class FIFO_Bits {
    FIFODT = REG_BIT_DEF(0, 31),
};

enum class Status_Flags {
    FLAG_CCRCERR = REG_BIT_DEF(0, 0),
    FLAG_DTCRCERR = REG_BIT_DEF(1, 1),
    FLAG_CMDTMOUT = REG_BIT_DEF(2, 2),
    FLAG_DTTMOUT = REG_BIT_DEF(3, 3),
    FLAG_TXURE = REG_BIT_DEF(4, 4),
    FLAG_RXORE = REG_BIT_DEF(5, 5),
    FLAG_CMDRECV = REG_BIT_DEF(6, 6),
    FLAG_CMDSEND = REG_BIT_DEF(7, 7),
    FLAG_DTEND = REG_BIT_DEF(8, 8),
    FLAG_STBITE = REG_BIT_DEF(9, 9),
    FLAG_DTBLKEND = REG_BIT_DEF(10, 10),
    FLAG_CMDRUN = REG_BIT_DEF(11, 11),
    FLAG_TXRUN = REG_BIT_DEF(12, 12),
    FLAG_RXRUN = REG_BIT_DEF(13, 13),
    FLAG_TFH = REG_BIT_DEF(14, 14),
    FLAG_RFH = REG_BIT_DEF(15, 15),
    FLAG_TFF = REG_BIT_DEF(16, 16),
    FLAG_RFF = REG_BIT_DEF(17, 17),
    FLAG_TFE = REG_BIT_DEF(18, 18),
    FLAG_RFE = REG_BIT_DEF(19, 19),
    FLAG_TXDTVAL = REG_BIT_DEF(20, 20),
    FLAG_RXDTVAL = REG_BIT_DEF(21, 21),
    FLAG_SDIOINT = REG_BIT_DEF(22, 22),
    FLAG_ATAEND = REG_BIT_DEF(23, 23),
};

enum class Clear_Flags {
    FLAG_CCRCERRC = REG_BIT_DEF(0, 0),
    FLAG_DTCRCERRC = REG_BIT_DEF(1, 1),
    FLAG_CMDTMOUTC = REG_BIT_DEF(2, 2),
    FLAG_DTTMOUTC = REG_BIT_DEF(3, 3),
    FLAG_TXUREC = REG_BIT_DEF(4, 4),
    FLAG_RXOREC = REG_BIT_DEF(5, 5),
    FLAG_CMDRECVC = REG_BIT_DEF(6, 6),
    FLAG_CMDSENDC = REG_BIT_DEF(7, 7),
    FLAG_DTENDC = REG_BIT_DEF(8, 8),
    FLAG_STBITEC = REG_BIT_DEF(9, 9),
    FLAG_DTBLKENDC = REG_BIT_DEF(10, 10),
    FLAG_SDIOINTC = REG_BIT_DEF(22, 22),
    FLAG_ATAENDC = REG_BIT_DEF(23, 23),
};

enum class Interrupt_Flags {
    FLAG_INTR_CCRCERR = REG_BIT_DEF(0, 0),
    FLAG_INTR_DTCRCERR = REG_BIT_DEF(1, 1),
    FLAG_INTR_CMDTMOUT = REG_BIT_DEF(2, 2),
    FLAG_INTR_DTTMOUT = REG_BIT_DEF(3, 3),
    FLAG_INTR_TXURE = REG_BIT_DEF(4, 4),
    FLAG_INTR_RXORE = REG_BIT_DEF(5, 5),
    FLAG_INTR_CMDRECV = REG_BIT_DEF(6, 6),
    FLAG_INTR_CMDSEND = REG_BIT_DEF(7, 7),
    FLAG_INTR_DTEND = REG_BIT_DEF(8, 8),
    FLAG_INTR_STBITE = REG_BIT_DEF(9, 9),
    FLAG_INTR_DTBLKEND = REG_BIT_DEF(10, 10),
    FLAG_INTR_CMDRUN = REG_BIT_DEF(11, 11),
    FLAG_INTR_TXRUN = REG_BIT_DEF(12, 12),
    FLAG_INTR_RXRUN = REG_BIT_DEF(13, 13),
    FLAG_INTR_TFH = REG_BIT_DEF(14, 14),
    FLAG_INTR_RFH = REG_BIT_DEF(15, 15),
    FLAG_INTR_TFF = REG_BIT_DEF(16, 16),
    FLAG_INTR_RFF = REG_BIT_DEF(17, 17),
    FLAG_INTR_TFE = REG_BIT_DEF(18, 18),
    FLAG_INTR_RFE = REG_BIT_DEF(19, 19),
    FLAG_INTR_TXDTVAL = REG_BIT_DEF(20, 20),
    FLAG_INTR_RXDTVAL = REG_BIT_DEF(21, 21),
    FLAG_INTR_SDIOINT = REG_BIT_DEF(22, 22),
    FLAG_INTR_ATAEND	 = REG_BIT_DEF(23, 23),
};

// Alias
enum class Interrupt_Type {
    CCRCERRIE = REG_BIT_DEF(0, 0),
    DTCRCERRIE = REG_BIT_DEF(1, 1),
    CMDTMOUTIE = REG_BIT_DEF(2, 2),
    DTTMOUTIE = REG_BIT_DEF(3, 3),
    TXUREIE = REG_BIT_DEF(4, 4),
    RXOREIE = REG_BIT_DEF(5, 5),
    CMDRECVIE = REG_BIT_DEF(6, 6),
    CMDSENDIE = REG_BIT_DEF(7, 7),
    DTENDIE = REG_BIT_DEF(8, 8),
    STBITEIE = REG_BIT_DEF(9, 9),
    DTBLKENDIE = REG_BIT_DEF(10, 10),
    CMDRUNIE = REG_BIT_DEF(11, 11),
    TXRUNIE = REG_BIT_DEF(12, 12),
    RXRUNIE = REG_BIT_DEF(13, 13),
    TFHIE = REG_BIT_DEF(14, 14),
    RFHIE = REG_BIT_DEF(15, 15),
    TFFIE = REG_BIT_DEF(16, 16),
    RFFIE = REG_BIT_DEF(17, 17),
    TFEIE = REG_BIT_DEF(18, 18),
    RFEIE = REG_BIT_DEF(19, 19),
    TXDTVALIE = REG_BIT_DEF(20, 20),
    RXDTVALIE = REG_BIT_DEF(21, 21),
    SDIOINTIE = REG_BIT_DEF(22, 22),
    ATAENDIE = REG_BIT_DEF(23, 23),
};

///////////////////////////// ENUMS /////////////////////////////

enum class Power_Control {
    POWER_OFF = 0,
    POWER_ON = 3,
};

enum class Bus_Width {
    WIDTH_1BIT,
    WIDTH_4BIT,
    WIDTH_8BIT,
};

enum class Clock_Edge {
    RISING_EDGE,
    FALLING_EDGE,
};

enum class Command_Response {
    RSP_NONE = 0,
    RSP_SHORT = 1,
    RSP_LONG = 3,
};

enum class Wait_Type {
    WT_NONE,
    WT_INTERRUPT,
    WT_COMPLETE,
};

enum class Response_Type {
    RESPONSE0,
    RESPONSE1,
    RESPONSE2,
    RESPONSE3,
};

enum class Block_Size {
    BYTES_1,
    BYTES_2,
    BYTES_4,
    BYTES_8,
    BYTES_16,
    BYTES_32,
    BYTES_64,
    BYTES_128,
    BYTES_256,
    BYTES_512,
    BYTES_1024,
    BYTES_2048,
    BYTES_4096,
    BYTES_8192,
    BYTES_16384,
};

enum Transfer_Mode {
    BLOCK,
    STREAM,
};

enum class Transfer_Direction {
    SDIO_TO_CARD,
    CARD_TO_SDIO,
};

enum class Read_Wait {
    DAT2,
    CLK,
};

enum class Command_Index {
    CMD0 = 0,					// GO_IDLE_STATE
    CMD1 = 1,					// SEND_OP_COND (MMC)
    CMD2 = 2,					// ALL_SEND_CID
    CMD3 = 3,					// SEND_RELATIVE_ADDR
    CMD4 = 4,					// SET_DSR
    CMD5 = 5,					// SEND_OP_COND (SD-IO)
    CMD6 = 6,					// SWITCH_FUNC
    ACMD6 = 6,					// ACMD
    CMD7 = 7,					// SELECT_DESELECT_CARD
    CMD8 = 8,					// SEND_IF_COND
    CMD9 = 9,					// SEND_CSD
    CMD10 = 10,					// SEND_CID
    CMD11 = 11,					// VOL_SWITCH
    CMD12 = 12,					// STOP_TRANSMISSION
    CMD13 = 13,					// SEND_STATUS
    ACMD13 = 13,				// ACMD
    CMD14 = 14,					// CHECK_BUS_TEST (MMC)
    CMD15 = 15,					// GO_INACTIVE_STATE
    CMD16 = 16,					// SET_BLOCKLEN
    CMD17 = 17,					// READ_SINGLE_BLOCK
    CMD18 = 18,					// READ_MULTIPLE_BLOCK
    CMD19 = 19,					// SEND_TUNING_BLOCK
    CMD20 = 20,					// SEND_TUNING_BLOCK (MMC)
    ACMD22 = 22,				// ACMD
    CMD23 = 23,					// SET_BLOCK_COUNT
    ACMD23 = 23,				// ACMD
    CMD24 = 24,					// WRITE_BLOCK
    CMD25 = 25,					// WRITE_MULTIPLE_BLOCK
    CMD27 = 27,					// PROGRAM_CSD
    CMD28 = 28,					// SET_WRITE_PROT
    CMD29 = 29,					// CLR_WRITE_PROT
    CMD30 = 30,					// SEND_WRITE_PROT
    CMD32 = 32,					// ERASE_WR_BLK_START
    CMD33 = 33,					// ERASE_WR_BLK_END
    CMD38 = 38,					// ERASE
    ACMD41 = 41,				// ACMD
    CMD42 = 42,					// LOCK_UNLOCK
    ACMD42 = 42,				// ACMD
    ACMD51 = 51,				// ACMD
    CMD55 = 55,					// APP_CMD
    CMD56 = 56,					// GEN_CMD
    INVALID = 60,
};

enum class Application_Command_Index {
    ACMD6 = 6,					// SET_BUS_WIDTH
    ACMD13 = 13,				// SD_STATUS
    ACMD22 = 22,				// SEND_NUM_WR_BLOCKS
    ACMD23 = 23,				// SET_WR_BLK_ERASE_COUNT
    ACMD41 = 41,				// SD_APP_OP_COND
    ACMD42 = 42,				// SET_CLR_CARD_DETECT
    ACMD51 = 51,				// SEND_SCR
};

enum class Card_Command_Class {
    BASIC = 0,
    BLOCK_READ = 2,
    BLOCK_WRITE = 4,
    ERASE = 5,
    WRITE_PROTECTION = 6,
    LOCK_CARD = 7,
    APPICATION_SPECIFIC = 8,
    IO_MODE = 9,
    SWITCH = 10,
};

enum class Lock_State {
    UNLOCK = 2,
    LOCK = 5,
};

enum class Interface_Version {
    INTERFACE_V1_1,
    INTERFACE_V2_0,
};

enum class Card_Type {
    SDCARD_STANDARD_CAPACITY,
    SDCARD_HIGH_CAPACITY,
    SD_IO_CARD,
    SD_IO_COMBO_CARD,
    SD_MMC,
    SD_MMC_HIGH_CAPACITY,
    SD_MMC_HIGH_SPEED,
};

enum class Transfer_State {
    IDLE,	// No transfer
    BUSY,	// Transfer in prgress
};

enum class Transfer_Method {
    METHOD_POLLING,
    METHOD_DMA,
};

enum class R1_Status {
    R1_OK = 0,
    AKE_SEQ_ERROR = 3,
    APP_CMD = 5,
    READY_FOR_DATA = 8,
    CURRENT_STATE = (0xF << 9),
    ERASE_RESET = 13,
    CARD_ECC_DISABLED = 14,
    WP_ERASE_SKIP = 15,
    CID_CSD_OVERWRITE = 16,
    OVERRUN = 17,
    UNDERRUN = 18,
    ERROR = 19,
    CC_ERROR = 20,
    CARD_ECC_FAILED = 21,
    ILLEGAL_COMMAND = 22,
    COM_CRC_ERROR = 23,
    LOCK_UNLOCK_FAILED = 24,
    CARD_IS_LOCKED = 25,
    WP_VIOLATION = 26,
    ERASE_PARAM = 27,
    ERASE_SEQ_ERROR = 28,
    BLOCK_LEN_ERROR = 29,
    ADDRESS_ERROR = 30,
    OUT_OF_RANGE = 31,
};

enum class R6_Status {
    ERROR = 13,
    ILLEGAL_COMMAND = 14,
    COM_CRC_ERROR = 15,
};

enum class Voltage_Validation {
    STANDARD_CAPACITY = 0,
    SWITCH_1_8V_CAPACITY = (1 << 24),
    HIGH_CAPACITY = (1 << 30),
};

enum class SDIO_Error_Type {
    OK = 0,
    ERROR,
    UNSUPPORTED_FUNCTION,
    INVALID_OPERATION,
    INVALID_PARAMETER,
    INVALID_VOLTAGE,
    START_BIT_ERROR,
    RX_FIFO_OVERRUN,
    TX_FIFO_UNDERRUN,
    DATA_TIMEOUT,
    RESPONSE_TIMEOUT,
    DATA_CRC_ERROR,
    COMMAND_CRC_ERROR,
    AUTHENTICATION_ERROR,
    ERASE_SEQUENCE_RESET,
    ECC_DISABLED,
    ERASE_SKIP,
    CSD_OVERWRITE,
    UNKNOWN_ERROR,
    CARD_CONTROLLER_ERROR,
    ECC_FAILED,
    INVALID_COMMAND,
    ILLEGAL_COMMAND,
    LOCK_UNLOCK_FAILED,
    WRITE_PROTECT_VIOLATION,
    INVALID_ERASE_BLOCKS,
    ERASE_SEQUENCE_ERROR,
    INVALID_BLOCK_LENGTH,
    INVALID_ADDRESS,
    COMMAND_OUT_OF_RANGE,
    INITIALIZATION_FAILED,
    INVALID_SELECTION,
    DMA_INSTANCE_ERROR,
    CMD0_FAILED,
    CMD2_FAILED,
    CMD3_FAILED,
    CMD9_FAILED,
    ACMD41_FAILED,
    CMD55_FAILED,
};


///////////////////////////// STRUCTURES /////////////////////////////

struct Card_Identification {
    volatile uint8_t manufacture_id;
    volatile uint16_t oem_id;
    volatile uint32_t name0;
    volatile uint8_t name1;
    volatile uint8_t revision;
    volatile uint32_t serial_number;
    volatile uint16_t manufacture_date;
    volatile uint8_t checksum;
};

struct Card_CSD {
    volatile uint8_t csd_structure;
    volatile uint8_t read_access_time;
    volatile uint8_t read_access_clock_cycles;
    volatile uint8_t transfer_speed;
    volatile uint16_t card_command_class;
    volatile uint8_t read_block_length;
    volatile uint8_t read_partial_block;
    volatile uint8_t write_unaligned_block;
    volatile uint8_t read_unaligned_block;
    volatile uint8_t dsr_available;
    volatile uint32_t device_size;				// CSD V1.0: 12 bits, CSD V2.0: 22 bits
    volatile uint8_t min_vdd_read;
    volatile uint8_t max_vdd_read;
    volatile uint8_t min_vdd_write;
    volatile uint8_t max_vdd_write;
    volatile uint8_t device_size_multiplier;	// CSD V1.0
    volatile uint8_t erase_block_enable;
    volatile uint8_t sector_size;
    volatile uint8_t write_protect_group_size;
    volatile uint8_t write_protect_group_enable;
    volatile uint8_t speed_factor;
    volatile uint8_t write_block_length;
    volatile uint8_t write_partial_block;
    volatile uint8_t file_format_group;
    volatile uint8_t copy_flag;					// OTP
    volatile uint8_t permanent_write_protect;
    volatile uint8_t temporary_write_protect;
    volatile uint8_t file_format;
    volatile uint8_t checksum;
};

struct Card_Info {
    Card_Identification cid;
    Card_CSD csd;
    Card_Type type;
    uint32_t capacity;
    uint32_t block_size;
    uint16_t relative_address;
};

struct SDIO_Pin_Config {
    gpio::GPIO_Base gpio_port;
    gpio::Pin_Number pin;
    gpio::Pin_Mode mode;
    gpio::Output_Speed speed;
};

struct SDIO_Pins {
    SDIO_Pin_Config cmd_pin;
    SDIO_Pin_Config ck_pin;
    SDIO_Pin_Config d0_pin;
    SDIO_Pin_Config d1_pin;
    SDIO_Pin_Config d2_pin;
    SDIO_Pin_Config d3_pin;
    SDIO_Pin_Config d4_pin;
    SDIO_Pin_Config d5_pin;
    SDIO_Pin_Config d6_pin;
    SDIO_Pin_Config d7_pin;
};

static constexpr uint32_t Check_Pattern = 0x000001AA;
static constexpr uint32_t Voltage_Window = 0x80100000;		// Host 3.3V request
static constexpr uint32_t Max_Voltage_Checks = 0x0000FFFF;	// Maximum number of voltage validation checks
static constexpr uint32_t Max_Data_Length = 0x01FFFFFF;		// Maximum length of data
static constexpr uint32_t Data_Timeout = 0xFFFFFFFF;		// Data timeout of the state machine
static constexpr uint16_t Init_Clock_Divider = 0x00C8;		// Divider during initialization
static constexpr uint16_t Regular_Clock_Divider = 0x0009;	// Divider for regular operation
static constexpr uint32_t All_Flags_Mask = 0x00C007FF;		// Mask of all the clear flags
static constexpr uint32_t SDCARD_HCS = 0x40000000;    		// HCS bit = 1 for voltage validation
static constexpr uint32_t Switch_1_8V_Capacity = 0x01000000;
static constexpr uint8_t RCA_Shift = 0x10;
static constexpr uint32_t Card_Locked = 0x02000000;
constexpr uint32_t SCR_Width_4 = 0x00040000;
constexpr uint32_t SCR_Width_1 = 0x00010000;
constexpr uint32_t All_R1_Error_Bits = 0xFDF9E008;
constexpr uint32_t FIFO_Half_Words = 0x00000008;
constexpr uint32_t FIFO_Half_Bytes = 0x00000020;
constexpr uint32_t PasswordKey1 = 0x01020600;
constexpr uint32_t PasswordKey2 = 0x03040506;
constexpr uint32_t CardStateMask = 0x0000000F;

constexpr uint32_t IDLE = 0;
constexpr uint32_t IDENTIFICATION = 1;
constexpr uint32_t STANDBY = 2;
constexpr uint32_t TRANSFER = 3;
constexpr uint32_t SEND_DATA = 4;
constexpr uint32_t RECEIVE_DATA = 5;
constexpr uint32_t PROGRAMMING = 6;
constexpr uint32_t DISCONNECTED = 7;

} // namespace sdio
