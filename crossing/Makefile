CC=gcc
CFLAGS=-pthread -O2 -Wall -Wextra
TARGET=crossing
SOURCES=logger.c tools.c simple.c crossing.c busy.c screen.c vehicle.c pedestrian.c main.c 

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) 

run: $(TARGET)
	./$(TARGET) simple

ext: $(TARGET)
	./$(TARGET) extended

clean:
	rm $(TARGET)

.PHONY: $(TARGET)
