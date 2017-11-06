CC=g++
CFLAGS= \
	-I.\
	-I~/opencv-3.1.0/include
LDFLAGS= \
	-L~/opencv-3.1.0/build/lib \
	-lopencv_core

all: archive

rel: CFLAGS += -O3
rel: all

analyzer.o : analyzer.cpp analyzer.h
	$(CC) $(CFLAGS) analyzer.cpp -c -o $@

archive: analyzer.o
	ar rvs libanalyzer.a analyzer.o

clean:
	rm *.o
	rm *.a
