TARGET=fencli
CC=gcc
CFLAGS=-std=c99 -Wall -O3

.PHONY: all clean

all: $(TARGET)

$(TARGET): fencli.c
	$(CC) $(CFLAGS) -o $(TARGET) fencli.c

clean:
	rm -f $(TARGET)
