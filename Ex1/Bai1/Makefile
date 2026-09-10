PROJECT = firmware

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb


SRC_DIR    = Src
BUILD_DIR  = Build
BIN_DIR    = Bin
LINKER_DIR = Linker

CMSIS_CORE_DIR   = Lib/STM32CubeF1/Drivers/CMSIS/Core/Include
CMSIS_DEVICE_DIR = Lib/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include
SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/startup.c


OBJECTS = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/startup.o


CFLAGS = \
	$(MCU) \
	-O0 \
	-g3 \
	-Wall \
	-Wextra \
	-ffreestanding \
	-ffunction-sections \
	-fdata-sections\
	-I$(CMSIS_CORE_DIR) \
    -I$(CMSIS_DEVICE_DIR)\
	-DSTM32F103xB
LDFLAGS = \
	$(MCU) \
	-T$(LINKER_DIR)/STM32F103C8.ld \
	-nostdlib \
	-Wl,--gc-sections \
	-Wl,-Map=$(BUILD_DIR)/$(PROJECT).map


all: directories $(BIN_DIR)/$(PROJECT).bin


directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)


$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/startup.o: $(SRC_DIR)/startup.c
	$(CC) $(CFLAGS) -c $< -o $@


$(BIN_DIR)/$(PROJECT).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@


$(BIN_DIR)/$(PROJECT).bin: $(BIN_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@


flash: all
	st-flash write $(BIN_DIR)/$(PROJECT).bin 0x08000000
	st-flash reset


clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(BIN_DIR)/*


.PHONY: all clean flash directories