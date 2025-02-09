############################## COMMON ##############################

# Compiler and Linker
CXX = arm-none-eabi-g++
CC = arm-none-eabi-gcc
LD = arm-none-eabi-g++
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy

# Target and output files
TARGET = MFL
OUTDIR = build
LINKER_SCRIPT = Linker/gd32f303re.ld

# C++ compiler flags
CXXFLAGS = -std=gnu++23 -mcpu=cortex-m4 -mthumb -Os $(INCLUDES) #-flto
CXXFLAGS += --specs=nosys.specs -Wall -Wextra -ffunction-sections -fdata-sections -fno-exceptions -fno-use-cxa-atexit -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -nostdlib --specs=nano.specs

# Linker flags
LDFLAGS	= -T$(LINKER_SCRIPT) -mcpu=cortex-m4 -mthumb -Os #-flto
LDFLAGS += --specs=nosys.specs -Wl,--gc-sections -Wl,--check-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align --specs=nano.specs
LDFLAGS += -lstdc++ -lgcc -lm -lc


############################## LIBRARY ##############################

# Source Directories
SRC_DIRS += MFL/Source/ADC MFL/Source/AFIO MFL/Source/BKP MFL/Source/CEE MFL/Source/COMMON MFL/Source/CORTEX MFL/Source/CRC MFL/Source/CTC
SRC_DIRS +=	MFL/Source/DAC MFL/Source/DBG MFL/Source/DMA MFL/Source/EXMC MFL/Source/EXTI MFL/Source/FMC MFL/Source/FWDGT MFL/Source/GPIO
SRC_DIRS +=	MFL/Source/I2C MFL/Source/OB MFL/Source/PMU MFL/Source/RCU MFL/Source/RTC MFL/Source/SDIO MFL/Source/SPI MFL/Source/STARTUP
SRC_DIRS +=	MFL/Source/STUBS MFL/Source/TIMER MFL/Source/USART MFL/Source/WWDGT CMSIS

# Include directories and files
INCLUDES = $(foreach dir, $(SRC_DIRS), -I$(dir))
INCLUDES += -I.

# Source files
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))

# Objects
OBJS = $(SRCS:%.cpp=$(OUTDIR)/%.o)

# Create output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)
	$(foreach dir, $(SRC_DIRS), mkdir -p $(OUTDIR)/$(dir);)

# Main library build rules
all: $(OUTDIR) $(TARGET).elf $(TARGET).bin

library: $(OUTDIR) $(TARGET).a

# Compile source files
$(OUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link main application
$(TARGET).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(OUTDIR)/$@

# Link binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(OUTDIR)/$< $(OUTDIR)/$@

# Link static library
$(TARGET).a: $(OBJS)
	$(AR) rsc $(OUTDIR)/$@ $(OBJS)


############################## USART DMA RX EXAMPLE ##############################

EXAMPLE_TARGET = example
USART_DMARX_OUTDIR = build_examples

# USART DMX RX source directories
USART_DMARX_SRC_DIRS = $(SRC_DIRS)
USART_DMARX_SRC_DIRS += Examples/USART_DMARX_IRQ

# USART DMX RX example includes
USART_DMAX_INCLUDES = $(foreach dir, $(USART_DMARX_SRC_DIRS), -I$(dir))
USART_DMAX_INCLUDES += -I.

# USART DMA RX source files
USART_DMARX_SRCS = $(foreach dir, $(USART_DMARX_SRC_DIRS), $(wildcard $(dir)/*.cpp))

# USART DMA RX objects
USART_DMARX_OBJS = $(USART_DMARX_SRCS:%.cpp=$(USART_DMARX_OUTDIR)/%.o)

# USART DMA RX output directory
$(USART_DMARX_OUTDIR):
	mkdir -p $(USART_DMARX_OUTDIR)
	$(foreach dir, $(USART_DMARX_SRC_DIRS), mkdir -p $(USART_DMARX_OUTDIR)/$(dir);)

# USART DMA RX build rules
usart_dmarx_irq_example: $(USART_DMARX_OUTDIR) $(EXAMPLE_TARGET).elf $(EXAMPLE_TARGET).bin

# Compile source files
$(USART_DMARX_OUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link main application
$(EXAMPLE_TARGET).elf:	$(USART_DMARX_OBJS)
	$(LD) $(USART_DMARX_OBJS) $(LDFLAGS) -o $(USART_DMARX_OUTDIR)/$@

# Link binary
$(EXAMPLE_TARGET).bin:	$(EXAMPLE_TARGET).elf
	$(OBJCOPY) -O binary $(USART_DMARX_OUTDIR)/$< $(USART_DMARX_OUTDIR)/$@


############################## CLEAN ##############################

clean:
	rm -rf $(OUTDIR) $(USART_DMARX_OUTDIR)

.PHONEY: all library usart_dmarx_irq_example clean
