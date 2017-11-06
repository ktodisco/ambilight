CC=g++
CFLAGS= \
	-I. \
	-I../led
LDFLAGS= \
	-L../led \
	-lled

SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXE=test_led

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
