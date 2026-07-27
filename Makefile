PIO     = $(USERPROFILE)/.platformio/packages
CC      = $(PIO)/toolchain-atmelavr/bin/avr-gcc.exe
OBJCOPY = $(PIO)/toolchain-atmelavr/bin/avr-objcopy.exe
AVRDUDE = $(PIO)/tool-avrdude/avrdude.exe

MCU     = m32
F_CPU   = 16000000UL
CFLAGS  = -mmcu=atmega32 -DF_CPU=$(F_CPU) -std=c99 -Wall -Os
LDFLAGS = -mmcu=atmega32

# Auto-discover sources
C_SOURCES := \
    $(wildcard src/*.c) \
    $(wildcard MCL/*.c) \
    $(wildcard MCL/*/*.c) \
    $(wildcard MCL/*/*/*.c) \
    $(wildcard HAL/*.c) \
    $(wildcard HAL/*/*.c) \
    $(wildcard HAL/*/*/*.c)

OBJS   := $(patsubst %.c,build/%.o,$(C_SOURCES))
TARGET := build/firmware

# Auto include folders (for #include "gpio.h" in MCL/GPIO/)
INCLUDE_DIRS := include src \
    $(sort $(dir $(wildcard MCL/*/*.h)) $(wildcard MCL/*/*/*.h)) \
    $(sort $(dir $(wildcard HAL/*/*.h)) $(wildcard HAL/*/*/*.h))
CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# --- Build pipeline stages, mirrored per source folder under build/ ---
# src/main.c        -> build/src/main.i   .s   .o
# MCL/GPIO/gpio.c    -> build/MCL/GPIO/gpio.i   .s   .o
#
# .c -> .i : preprocessing   (macro expansion, #include text replacement)
# .i -> .s : compiling       (C code -> AVR assembly)
# .s -> .o : assembling      (assembly -> machine code / object file)
# .o -> .elf -> .hex : linking + hex conversion (rules above, top of file)

build/%.i: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -E $< -o $@

build/%.s: build/%.i
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -S $< -o $@

build/%.o: build/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Keep .i/.s intermediates around for inspection instead of letting make
# delete them as "just intermediate" files in the chain.
.SECONDARY:

clean:
	-if exist build rmdir /s /q build

flash: $(TARGET).hex
	$(AVRDUDE) -c usbasp -p $(MCU) -U flash:w:$<:i

.PHONY: all clean flash