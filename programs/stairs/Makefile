PROJECT=stairs
LibPath =/home/humanoides/humanoids/cm510_controller_fw
########################################################
# afegir tots els fitxers que s'han de compilar aquí
########################################################
SOURCES= stairs_last.c small_steps.c
#SOURCES= calibrate.c



OBJS=$(SOURCES:.c=.o)
SRC_DIR=./
DEV_DIR=$(LibPath)/dyn_devices/
COMM_DIR=$(LibPath)/communications/
CONT_DIR=$(LibPath)/controllers/
MAN_DIR=$(LibPath)/motion/
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2561

LIBS=$(CONT_DIR)lib/libcontrollers.a $(MAN_DIR)lib/libmotion_manager.a $(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a

INCLUDE_DIRS=-I$(DEV_DIR)include -I$(COMM_DIR)include -I$(CONT_DIR)include -I$(MAN_DIR)include

CFLAGS=-mmcu=$(MMCU) -Wall -Os $(defines) -DF_CPU=16000000UL -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

LDFLAGS=-mmcu=$(MMCU) -Wl,-Map=$(PROJECT).map -DF_CPU=16000000UL

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

.PHONY: all

all: communications dyn_devices controllers motion_manager $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(PROJECT).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROJECT).elf
%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDE_DIRS) -o $@ $<

communications:
	$(MAKE) -C $(COMM_DIR)

dyn_devices:
	$(MAKE) -C $(DEV_DIR)

controllers:
	$(MAKE) -C $(CONT_DIR)

motion_manager:
	$(MAKE) -C $(MAN_DIR)

download: $(MAIN_OUT_HEX)
	fw_downloader -d /dev/ttyUSB0 -f ./$(PROJECT).hex -p cm510

clean:
	-rm *.o
	-rm *.map
	-rm *.elf
	-rm *.hex
	-rm *~
	-rm $(OBJS)
