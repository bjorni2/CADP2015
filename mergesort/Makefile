CC=gcc
CFLAGS=-pthread -O2 -Wall -Wextra
TARGET=merge
SOURCES=main.c

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) 

clean:
	rm $(TARGET)

.PHONY: $(TARGET)
