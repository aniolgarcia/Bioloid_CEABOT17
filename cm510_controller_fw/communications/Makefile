PROJECT=libcomm
########################################################
# afegir tots els fitxers que s'han de compilar aquí
########################################################
SOURCES=src/dynamixel.c src/dynamixel_master.c src/serial_console.c

OBJS=$(SOURCES:.c=.o)
SRC_DIR=./src/
INCLUDE_DIR=./include/
CONT_DIR=../controllers
BUILD_DIR=./build/
BIN_DIR=./bin/
LIB_DIR=./lib/
CC=avr-gcc
OBJCOPY=avr-ar
MMCU=atmega2561

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

ARFLAGS=rsc

.PHONY: show_banner all

all: show_banner $(PROJECT).a 

show_banner:
	@echo "------------------------------------------------------";
	@echo "       _____                                          ";
	@echo "      /  _  \                                         ";
	@echo "      | |_| |               The Humanoid Lab          ";
	@echo "  ____\_____/____                                     ";
	@echo " /               \    http://apollo.upc.es/humanoide/ ";
	@echo "/   _         _   \                                   ";
	@echo "|  | |       | |  |            $(PROJECT)             ";
	@echo "|  | |       | |  |                                   ";
	@echo "------------------------------------------------------";

$(PROJECT).a: $(OBJS)
	mkdir -p lib
	$(OBJCOPY) $(ARFLAGS) $(LIB_DIR)$(PROJECT).a $(OBJS)
%.o:%.c
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) -I$(CONT_DIR)/include -o $@ $<
examples:
	$(MAKE) -C src/examples	
clean:
	-rm $(LIB_DIR)$(PROJECT).*
	-rm $(OBJS)
	$(MAKE) -C src/examples clean
