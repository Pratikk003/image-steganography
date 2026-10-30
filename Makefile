# Makefile for Image Steganography project

CC      := gcc
CFLAGS  := -Wall -Wextra -g
TARGET  := stego
SRCS    := test_encode.c encode.c decode.c
OBJS    := $(SRCS:.c=.o)
HDRS    := $(wildcard *.h)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Usage examples:
#   ./stego -e beautiful.bmp secret.txt stego.bmp
#   ./stego -d stego.bmp output.txt
help:
	@echo "Targets:"
	@echo "  make        - build the project ($(TARGET))"
	@echo "  make clean  - remove object files and the $(TARGET) binary"
	@echo "Usage:"
	@echo "  ./$(TARGET) -e <src.bmp> <secret.txt> [stego.bmp]"
	@echo "  ./$(TARGET) -d <stego.bmp> [output.txt]"

clean:
	rm -f $(OBJS) $(TARGET)
