###############################################################################
# Makefile for the project Sopriazenie
###############################################################################

## General Flags
PROJECT = Sopriazenie
MCU = atmega168
TARGET = summator.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2   -std=gnu99                                                                      -DF_CPU=14745600UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-section-start=.also_var=0x3B00  -Wl,-section-start=.bootloader=0x3960 -Wl,-Map=summator.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Libraries
LIBS = -lm -lc 

## Objects that must be built in order to link
OBJECTS = main.o spi.o potenciometer.o vout.o rout.o freq_dot.o mbfanction0.o mbclient.o crc16.o usart.o settings.o errors_led.o dot.o util.o omnicomm.o reply.o crc8.o adc.o fout.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) summator.hex summator.eep summator.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi.o: ../spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

potenciometer.o: ../potenciometer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

vout.o: ../vout.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rout.o: ../rout.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

freq_dot.o: ../freq_dot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbfanction0.o: ../mbfanction0.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mbclient.o: ../mbclient.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

crc16.o: ../crc16.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

settings.o: ../settings.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

errors_led.o: ../errors_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dot.o: ../dot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

util.o: ../util.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

omnicomm.o: ../omnicomm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

reply.o: ../reply.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

crc8.o: ../crc8.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

adc.o: ../adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

fout.o: ../fout.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) summator.elf dep/* summator.hex summator.eep summator.lss summator.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

