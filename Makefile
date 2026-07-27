# Makefile для BitOS-32

CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -c
LD = ld
LDFLAGS = -m elf_i386 -T linker2.ld

TARGET = BitOS.elf
OBJS = boot.o kenel1.o read_keyboard.o ata.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot.o: boot.asm
	nasm -f elf32 $< -o $@

kenel1.o: kenel1.c
	$(CC) $(CFLAGS) $< -o $@

read_keyboard.o: read_keyboard.c
	$(CC) $(CFLAGS) $< -o $@

ata.o: ata.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
