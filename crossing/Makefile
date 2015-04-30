CC=gcc
CFLAGS=-pthread -O2 -Wall -Wextra
TARGET=merge
SOURCES=simple.c

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) 

clean:
	rm $(TARGET)

.PHONY: $(TARGET)
