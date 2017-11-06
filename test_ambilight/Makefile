CC=g++
CFLAGS= \
	-I. \
	-I../analyzer \
	-I~/opencv-3.1.0/include
LDFLAGS= \
	-L~/opencv-3.1.0/build/lib \
	-L../analyzer \
	-lopencv_videoio \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_imgproc \
	-lopencv_imgcodecs \
	-lpthread \
	-lanalyzer

SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXE=test_ambilight

all: $(SOURCES) $(EXE)

rel: CFLAGS += -O3
rel: all

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm *.o
	rm $(EXE)
