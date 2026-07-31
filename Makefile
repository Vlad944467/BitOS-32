CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -c
LD = ld
LDFLAGS = -m elf_i386 -T boot/linker.ld

TARGET = sysb37.elf
OBJS = boot.o kenel1.o commands.o read_keyboard.o ata.o video.o elf.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot.o: boot/boot.asm
	nasm -f elf32 $< -o $@

kenel1.o: kernel/kenel1.c
	$(CC) $(CFLAGS) $< -o $@

commands.o: kernel/commands.c
	$(CC) $(CFLAGS) $< -o $@

read_keyboard.o: kernel/read_keyboard.c
	$(CC) $(CFLAGS) $< -o $@

ata.o: kernel/ata.c
	$(CC) $(CFLAGS) $< -o $@

video.o: kernel/video.c
	$(CC) $(CFLAGS) $< -o $@

elf.o: kernel/elf.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
