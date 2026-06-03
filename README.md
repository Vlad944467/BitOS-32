# BitOS-32

**32-битная монолитная операционная система, написанная на C и Assembly**

## 🚀 Возможности

- ✅ 32-битный защищённый режим
- ✅ Загрузка по стандарту Multiboot (GRUB/QEMU)
- ✅ Поддержка клавиатуры PS/2
- ✅ Текстовый режим VGA (80x25, 16 цветов)
- ✅ Звук через PC Speaker
- ✅ Встроенные команды: `help`, `clear`, `reboot`, `echo`, `win`, `cat`, `fetch`

## 🛠️ Сборка и запуск

### Ручная сборка

```bash
nasm -f elf32 boot.s -o boot.o
gcc -m32 -ffreestanding -nostdlib -c kernel.c -o kernel.o
ld -m elf_i386 -T linker.ld -o bitos.bin boot.o kernel.o
