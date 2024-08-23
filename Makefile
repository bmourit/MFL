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

# Source Sirectories
SRC_DIRS = Source/ADC Source/AFIO Source/CEE Source/COMMON Source/CORTEX Source/DBG Source/DMA Source/EXTI
SRC_DIRS +=	Source/FMC Source/FWDGT Source/GPIO Source/OB Source/PMU Source/RCU Source/RTC Source/SPI
SRC_DIRS +=	Source/STARTUP Source/TIMER Source/USART Source/WWDGT
SRC_DIRS +=	CMSIS

# Include directories and files
INCLUDES = $(foreach dir, $(SRC_DIRS), -I$(dir))
INCLUDES += -I.

# Source files
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
SRCS += main.cpp

# Objects
OBJS = $(SRCS:%.cpp=$(OUTDIR)/%.o)

# C++ compiler flags
CXXFLAGS = -std=gnu++20 -mcpu=cortex-m4 -mthumb -Os -g $(INCLUDES)
CXXFLAGS += --specs=nosys.specs -Wall -Wextra -ffunction-sections -fdata-sections -fno-exceptions -fno-use-cxa-atexit -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -nostdlib --specs=nano.specs

# Linker flags
LDFLAGS	= -T$(LINKER_SCRIPT) -mcpu=cortex-m4 -mthumb -Os -g
LDFLAGS += --specs=nosys.specs -Wl,--gc-sections -Wl,--check-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align --specs=nano.specs
LDFLAGS += -lstdc++ -lgcc -lm -lc

# Create output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)
	$(foreach dir, $(SRC_DIRS), mkdir -p $(OUTDIR)/$(dir);)

# Build rules
all: $(OUTDIR) $(TARGET).elf $(TARGET).bin

$(OUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(OUTDIR)/$@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(OUTDIR)/$< $(OUTDIR)/$@

clean:
	rm -rf $(OUTDIR)

.PHONEY: all clean
