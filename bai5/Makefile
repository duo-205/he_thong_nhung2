TARGET = bai5

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

PROJECT_DIR = .
LIB_DIR = $(HOME)/STM32/Libraries/STM32F10x_StdPeriph_Lib_V3.6.0

CMSIS_DIR = $(LIB_DIR)/Libraries/CMSIS
SPL_DIR = $(LIB_DIR)/Libraries/STM32F10x_StdPeriph_Driver

INC_DIR = $(PROJECT_DIR)/inc
SRC_DIR = $(PROJECT_DIR)/src
BUILD_DIR = $(PROJECT_DIR)/build

CFLAGS = \
	-mcpu=cortex-m3 \
	-mthumb \
	-mfloat-abi=soft \
	-ffunction-sections \
	-fdata-sections \
	-Os \
	-g \
	-Wall \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-I$(INC_DIR) \
	-I$(SPL_DIR)/inc \
	-I$(CMSIS_DIR)/CM3/CoreSupport \
	-I$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x

LDFLAGS = \
	-mcpu=cortex-m3 \
	-mthumb \
	-mfloat-abi=soft \
	-Tstm32f103c8.ld \
	-Wl,--gc-sections \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
	-specs=nano.specs \
	-specs=nosys.specs

C_SOURCES = \
	$(SRC_DIR)/main.c \
	$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
	$(SPL_DIR)/src/misc.c \
	$(SPL_DIR)/src/stm32f10x_rcc.c \
	$(SPL_DIR)/src/stm32f10x_flash.c \
	$(SPL_DIR)/src/stm32f10x_gpio.c \
	$(SPL_DIR)/src/stm32f10x_tim.c \
	$(SPL_DIR)/src/stm32f10x_usart.c

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SOURCES)))

VPATH = \
	$(SRC_DIR) \
	$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x \
	$(SPL_DIR)/src


all: $(BUILD_DIR)/$(TARGET).bin
	@echo
	@echo "===== BUILD SUCCESS ====="
	@$(SIZE) $(BUILD_DIR)/$(TARGET).elf
	@echo

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) startup_stm32f10x_md.s stm32f103c8.ld
	$(CC) $(CFLAGS) $(LDFLAGS) \
		$(OBJECTS) startup_stm32f10x_md.s \
		-o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
