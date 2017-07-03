#AVR-GCC  Makefile

PROJECT=libcontrollers
SOURCES=src/cm510.c src/gpio.c src/adc.c src/buzzer.c src/user_time.c
OBJS=$(SOURCES:.c=.o)
SRC_DIR=./src/ 
BIN_DIR=./build/
LIB_DIR=./lib/
DEV_DIR=../dyn_devices/
COMM_DIR=../communications/
MAN_DIR=../motion/
CONT_DIR=./
CC=avr-gcc
OBJCOPY=avr-ar
MMCU=atmega2561

INC_DIRS=-I$(CONT_DIR)include/ -I$(COMM_DIR)include -I$(DEV_DIR)include -I$(MAN_DIR)include

LIBS=$(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a $(MAN_DIR)lib/libmotion_manager.a

CFLAGS=-mmcu=$(MMCU) -Wall -O3 -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

ARFLAGS=rsc

.PHONY: all show_banner clean

all: show_banner communications dyn_devices motion_manager $(PROJECT).a

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

$(PROJECT).a: ${OBJS}
	mkdir -p lib
	$(OBJCOPY) $(ARFLAGS) ${LIB_DIR}$(PROJECT).a $(OBJS)

%.o: %.c 
	$(CC) -c $(CFLAGS) $(INC_DIRS) -o $@ $<

communications:
	$(MAKE) -C ../communications

dyn_devices:
	$(MAKE) -C ../dyn_devices

motion_manager:
	$(MAKE) -C ../motion

examples: 
	$(MAKE) -C src/examples
	$(MAKE) -C ../communications examples
	$(MAKE) -C ../dyn_devices examples
	$(MAKE) -C ../motion examples

download: 
	$(MAKE) -C src/examples download

clean:
	rm -f ${LIB_DIR}$(PROJECT).a
	rm -f $(OBJS) 
	$(MAKE) -C src/examples clean
	$(MAKE) -C ../communications clean
	$(MAKE) -C ../dyn_devices clean
	$(MAKE) -C ../motion clean
