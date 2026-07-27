# Bootloader

Загрузчик для BitOS-32, написанный на ассемблере (NASM).

---

## 📁 Файлы

| Файл | Описание |
|------|----------|
| `bootloader.s` | Основной код загрузчика |
| `grub.cfg` | Конфигурация для GRUB (опционально) |

---

## 🔧 Сборка

```bash
nasm -f elf32 bootloader.s -o bootloader.o
