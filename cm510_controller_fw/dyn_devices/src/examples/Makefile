PROJECT_SERVO=servos_ex
PROJECT_EXP_BOARD=exp_board_ex
########################################################
# afegir tots els fitxers que s'han de compilar aquí
########################################################
SOURCES_SERVO=servos_ex.c
SOURCES_EXP_BOARD=exp_board_ex.c

OBJS_SERVO=$(SOURCES_SERVO:.c=.o)
OBJS_EXP_BOARD=$(SOURCES_EXP_BOARD:.c=.o)
SRC_DIR=./src/
DEV_DIR=../../
COMM_DIR=../../../communications/
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2561

LIBS=$(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a

INCLUDE_DIRS=-I$(DEV_DIR)include -I$(COMM_DIR)include

CFLAGS=-mmcu=$(MMCU) -Wall -Os $(defines) -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

LDFLAGS=-mmcu=$(MMCU) -Wl,-Map=$(PROJECT).map -DF_CPU=16000000UL

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

.PHONY: all

all: communications dyn_devices $(PROJECT_SERVO).hex ${PROJECT_EXP_BOARD}.hex

$(PROJECT_SERVO).hex: $(PROJECT_SERVO).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(PROJECT_SERVO).elf: $(OBJS_SERVO) $(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a
	$(CC) $(LDFLAGS) $(OBJS_SERVO) $(LIB_DIRS) $(LIBS) -o $(PROJECT_SERVO).elf
$(PROJECT_EXP_BOARD).hex: $(PROJECT_EXP_BOARD).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(PROJECT_EXP_BOARD).elf: $(OBJS_EXP_BOARD) $(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a
	$(CC) $(LDFLAGS) $(OBJS_EXP_BOARD) $(LIB_DIRS) $(LIBS) -o $(PROJECT_EXP_BOARD).elf
%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDE_DIRS) -o $@ $<

communications:
	$(MAKE) -C $(COMM_DIR)

dyn_devices:
	$(MAKE) -C $(DEV_DIR)

download: $(MAIN_OUT_HEX)
	fw_downloader -d /dev/ttyUSB0 -f ./src/examples/comm_ex.hex -p new

clean:
	-rm $(PROJECT_SERVO).hex
	-rm $(PROJECT_SERVO).elf
	-rm $(OBJS_SERVO)
	-rm $(PROJECT_EXP_BOARD).hex
	-rm $(PROJECT_EXP_BOARD).elf
	-rm $(OBJS_EXP_BOARD)
