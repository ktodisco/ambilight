CC=g++
CFLAGS= \
	-I.
LDFLAGS=

all: archive

rel: CFLAGS += -O3
rel: all

lpd8806.o: lpd8806.cpp lpd8806.h
	$(CC) $(CFLAGS) lpd8806.cpp -c -o $@

archive: lpd8806.o
	ar rvs libled.a lpd8806.o

clean:
	rm *.o
	rm *.a
