PROJECT=comm_ex
########################################################
# afegir tots els fitxers que s'han de compilar aquí
########################################################
SOURCES=main.c

OBJS=$(SOURCES:.c=.o)
SRC_DIR=./src/
INCLUDE_DIR=../../include/
COMM_DIR=../../lib/
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2561

LIBS=$(COMM_DIR)libcomm.a

CFLAGS=-mmcu=$(MMCU) -Wall -Os $(defines) -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

LDFLAGS=-mmcu=$(MMCU) -Wl,-Map=$(PROJECT).map -DF_CPU=16000000UL

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

.PHONY: all

all: $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(PROJECT).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIB_DIRS) $(LIBS) -o $(PROJECT).elf
%.o:%.c
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

download: $(MAIN_OUT_HEX)
	avrdude -c avr109 -P /dev/ttyUSB0 -b 19200 -p m128 -U flash:w:$(PROJECT).hex
	avrdude -c avr109 -P /dev/ttyUSB0 -b 19200 -p m128 -U eeprom:w:$(PROJECT)_eeprom.hex

clean:
	-rm $(PROJECT).*
	-rm $(OBJS)
