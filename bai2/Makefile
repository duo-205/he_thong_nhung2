TARGET = bai2

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

STD = $(HOME)/STM32/Libraries/STM32F10x_StdPeriph_Lib_V3.6.0
CMSIS = $(STD)/Libraries/CMSIS
SPL = $(STD)/Libraries/STM32F10x_StdPeriph_Driver

CFLAGS = -mcpu=cortex-m3 -mthumb \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         -Wall -O0 -g3 \
         -ffunction-sections -fdata-sections \
         -Iinc \
         -I$(CMSIS)/CM3/CoreSupport \
         -I$(CMSIS)/CM3/DeviceSupport/ST/STM32F10x \
         -I$(SPL)/inc

ASFLAGS = -mcpu=cortex-m3 -mthumb

LDFLAGS = -mcpu=cortex-m3 -mthumb \
          -Tstm32f103c8.ld \
          -specs=nano.specs \
          -specs=nosys.specs \
          -Wl,--gc-sections \
          -Wl,-Map=build/$(TARGET).map

BUILD = build

C_OBJECTS = \
    $(BUILD)/main.o \
    $(BUILD)/system_stm32f10x.o \
    $(BUILD)/misc.o \
    $(BUILD)/stm32f10x_rcc.o \
    $(BUILD)/stm32f10x_flash.o \
    $(BUILD)/stm32f10x_gpio.o

STARTUP = startup_stm32f10x_md.s
STARTUP_OBJ = $(BUILD)/startup_stm32f10x_md.o

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin
	@echo
	@echo "===== BUILD SUCCESS ====="
	$(SIZE) $(BUILD)/$(TARGET).elf

$(BUILD)/$(TARGET).elf: $(C_OBJECTS) $(STARTUP_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/main.o: src/main.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/system_stm32f10x.o: $(CMSIS)/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/misc.o: $(SPL)/src/misc.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/stm32f10x_rcc.o: $(SPL)/src/stm32f10x_rcc.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/stm32f10x_flash.o: $(SPL)/src/stm32f10x_flash.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/stm32f10x_gpio.o: $(SPL)/src/stm32f10x_gpio.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup_stm32f10x_md.o: $(STARTUP)
	@mkdir -p $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)/*

.PHONY: all clean
