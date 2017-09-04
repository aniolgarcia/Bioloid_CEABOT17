# computer code
INPUT_FILE  = computer.cpp
OUTPUT_FILE = computer
LIBS        = -lpthread -L/usr/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab -L/usr/local/lib/iridrivers -Wl,-rpath=/usr/local/lib/iridrivers -ldetectqrcode -lcomm -liriutils
INCLUDES    = -I. -I/usr/include/opencv -I/usr/local/include/iridrivers

# microcontroller code
CM510_PROJECT = cm510
CM510_SOURCES = cm510.c

CM510_OBJS=$(CM510_SOURCES:.c=.o)
CM510_SRC_DIR=$(HOME)/humanoids/cm510_controller_fw
CM510_DEV_DIR=$(CM510_SRC_DIR)/dyn_devices/
CM510_COMM_DIR=$(CM510_SRC_DIR)/communications/
CM510_CONT_DIR=$(CM510_SRC_DIR)/controllers/
CM510_MAN_DIR=$(CM510_SRC_DIR)/motion/
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=atmega2561

LIBS=$(CONT_DIR)lib/libcontrollers.a $(MAN_DIR)lib/libmotion_manager.a $(DEV_DIR)lib/libdyn_devices.a $(COMM_DIR)lib/libcomm.a

INCLUDE_DIRS=-I$(DEV_DIR)include -I$(COMM_DIR)include -I$(CONT_DIR)include -I$(MAN_DIR)include

CFLAGS=-mmcu=$(MMCU) -Wall -Os $(defines) -DF_CPU=16000000UL -D__REAL__ -gdwarf-2 -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wstrict-prototypes

LDFLAGS=-mmcu=$(MMCU) -Wl,-Map=$(CM510_PROJECT).map -DF_CPU=16000000UL

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

all: $(OUTPUT_FILE) communications dyn_devices controllers motion_manager $(CM510_PROJECT).hex
$(OUTPUT_FILE): $(INPUT_FILE) ueyeinterface.o
	$(CXX) $(INPUT_FILE) ueyeinterface.o $(LIBS) $(INCLUDES) -o $(OUTPUT_FILE) -D__REAL__ -lueye_api

$(OUTPUT_FILE)_sim: $(INPUT_FILE) ueyeinterface.o
	$(CXX) $(INPUT_FILE) $(LIBS) $(INCLUDES) -o $(OUTPUT_FILE)_sim

ueyeinterface.o: ueyeinterface.h ueyeinterface.cpp
	g++ -c ueyeinterface.cpp -o ueyeinterface.o -O2 -Wall

$(CM510_PROJECT).hex: $(CM510_PROJECT).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@
$(CM510_PROJECT).elf: $(CM510_OBJS)
	$(CC) $(LDFLAGS) $(CM510_OBJS) $(CM510_LIBS) -o $(CM510_PROJECT).elf
%.o:%.c
	$(CC) -c $(CFLAGS) $(CM510_INCLUDE_DIRS) -o $@ $<

communications:
	$(MAKE) -C $(CM510_COMM_DIR)

dyn_devices:
	$(MAKE) -C $(CM510_DEV_DIR)

controllers:
	$(MAKE) -C $(CM510_CONT_DIR)

motion_manager:
	$(MAKE) -C $(CM510_MAN_DIR)

download: $(MAIN_OUT_HEX)
	fw_downloader -d /dev/ttyS1 -f ./$(CM510_PROJECT).hex -p cm510

clean:
	-rm $(CM510_PROJECT).map
	-rm $(CM510_PROJECT).hex
	-rm $(CM510_PROJECT).elf
	-rm $(CM510_OBJS)
	$(RM) $(OUTPUT_FILE)

.PHONY: all clean
