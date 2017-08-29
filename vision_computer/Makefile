INPUT_FILE  = grab.c
OUTPUT_FILE = grab
LIBS        = -L/usr/local/lib/iridrivers -liriutils -lcomm -lpthread -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lueye_api -lzbar
INCLUDES    = -I. -I/usr/local/include/iridrivers -I/usr/include/opencv2

all:
	$(CXX) $(INPUT_FILE) $(LIBS) $(INCLUDES) -o $(OUTPUT_FILE)

clean:
	$(RM) $(OUTPUT_FILE)

.PHONY: all clean
