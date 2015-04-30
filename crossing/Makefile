CC=gcc
CFLAGS=-pthread -O2 -Wall -Wextra
TARGET=crossing
SOURCES=logger.c simple.c crossing.c busy.c screen.c vehicle.c pedestrian.c main.c 

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) 

clean:
	rm $(TARGET)

.PHONY: $(TARGET)
