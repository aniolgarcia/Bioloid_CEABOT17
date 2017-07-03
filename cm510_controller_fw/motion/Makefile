PROJECT=libmotion_manager
SOURCES=src/motion_manager.c src/action.c src/motion_pages.c src/balance.c src/mtn_library.c src/pan_tilt.c 
OBJS=$(SOURCES:.c=.o)
SRC_DIR=./src/ 
BIN_DIR=./build/
LIB_DIR=./lib/
COMM_DIR=../communications/
DEV_DIR=../dyn_devices/
CONT_DIR=../controllers/
CC=avr-gcc
OBJCOPY=avr-ar
MMCU=atmega2561

INC_DIRS=-I./include/ -I$(DEV_DIR)include/ -I$(COMM_DIR)include/ -I$(CONT_DIR)include/

LIBS=$(CONT_DIR)lib/libcontrollers.a $(COMM_DIR)lib/libcomm.a $(DEV_DIR)lib/libdyn_devices.a

CFLAGS=-mmcu=$(MMCU) -Wall -O3 -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

ARFLAGS= rsc

.PHONY: all show_banner clean

all: show_banner communications dyn_devices $(PROJECT).a

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

communications:
	$(MAKE) -C $(COMM_DIR)

dyn_devices:
	$(MAKE) -C $(DEV_DIR)

examples:
	$(MAKE) -C src/examples

%.o: %.c 
	$(CC) -c $(CFLAGS) ${INC_DIRS} -o $@ $<

clean:
	rm -f ${LIB_DIR}$(PROJECT).a
	rm -f $(OBJS) 
	$(MAKE) -C src/examples clean
