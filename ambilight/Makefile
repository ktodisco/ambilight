CC=g++
CFLAGS= \
	-I../analyzer \
	-I../led \
	-I~/opencv-3.1.0/include
LDFLAGS= \
	-L~/opencv-3.1.0/build/lib \
	-L../analyzer \
	-L../led \
	-lopencv_videoio \
	-lopencv_core \
	-lopencv_imgproc \
	-lpthread \
	-lled \
	-lanalyzer

all: exe

rel: CFLAGS += -O3
rel: all

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp -c -o $@

exe: main.o
	$(CC) main.o -o ambilight $(LDFLAGS)

clean:
	rm *.o
	rm ambilight
