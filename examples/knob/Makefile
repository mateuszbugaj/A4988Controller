A4988_LIBRARY_DIR := ../../
A4988_LIBRARY_NAME := liba4988
A4988_LIBRARY_INC_DIR := $(A4988_LIBRARY_DIR)inc
MCU=atmega168
F_CPU=8000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I$(A4988_LIBRARY_INC_DIR) -I./inc
TARGET=knob
SRCDIR := src
INCDIR := ./inc
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(SRCDIR)/%.o, $(SOURCES))
DEPS := $(patsubst $(SRCDIR)/%.c, $(SRCDIR)/%.d, $(SOURCES))

# rules
.PHONY: all flash clean

all: ${TARGET}.hex

-include $(DEPS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	${CC} -c -MMD ${CFLAGS} -I$(INCDIR) $< -o $@

${TARGET}.bin: $(OBJECTS) $(A4988_LIBRARY_DIR)$(A4988_LIBRARY_NAME).a
	${CC} ${CFLAGS} -o $@ $^

${TARGET}.hex: ${TARGET}.bin
	${OBJCOPY} -j .text -j .data -O ihex $< $@

flash: ${TARGET}.hex
	avrdude -p ${MCU} -c usbasp -U flash:w:$<:i -F -P usb -B 4

clean:
	rm -f *.bin *.hex $(SRCDIR)/*.o $(SRCDIR)/*.d

flash_and_clean: all flash clean