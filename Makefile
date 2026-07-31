CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -c
LD = ld
LDFLAGS = -m elf_i386 -T boot/linker.ld

TARGET = pmocero.elf
OBJS = boot.o PMocero.o commands.o read_keyboard.o ata.o video.o elf.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot.o: boot/boot.asm
	nasm -f elf32 $< -o $@

PMocero.o: kernel/PMocero.c
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

run: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

run-hda: $(TARGET)
	qemu-system-i386 -kernel $(TARGET) -hda fat16.img

help:
	@echo "=== PMocero Makefile ==="
	@echo "make          - Собрать ядро"
	@echo "make clean    - Удалить объектные файлы"
	@echo "make rebuild  - Пересобрать с нуля"
	@echo "make run      - Запустить в QEMU"
	@echo "make run-hda  - Запустить с диском fat16.img"
