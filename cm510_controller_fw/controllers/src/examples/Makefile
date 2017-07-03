PROJECT=cm510_ex
########################################################
# afegir tots els fitxers que s'han de compilar aquí
########################################################
SOURCES=main.c

OBJS=$(SOURCES:.c=.o)
SRC_DIR=./src/
DEV_DIR=../../../dyn_devices/
COMM_DIR=../../../communications/
MAN_DIR=../../../motion/
CONT_DIR=../../
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2561

INCLUDE_DIRS=-I$(CONT_DIR)include/ -I$(COMM_DIR)include -I$(MAN_DIR)include -I$(DEV_DIR)include

LIBS=$(CONT_DIR)lib/libcontrollers.a $(MAN_DIR)lib/libmotion_manager.a $(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a

CFLAGS=-mmcu=$(MMCU) -Wall -Os $(defines) -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

LDFLAGS=-mmcu=$(MMCU) -Wl,-Map=$(PROJECT).map -DF_CPU=16000000UL

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

.PHONY: all

all: $(PROJECT).hex 

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(PROJECT).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROJECT).elf
%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDE_DIRS) -o $@ $<

download: $(MAIN_OUT_HEX)
	fw_downloader -d /dev/ttyUSB0 -f $(PROJECT).hex -p new

clean:
	-rm $(PROJECT).*
	-rm $(OBJS)
