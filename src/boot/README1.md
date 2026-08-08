# Bootloader

Загрузчик для OceroOS-32, написанный на ассемблере (NASM).

---

## Файлы

| Файл | Описание |
|------|----------|
| `bootloader.s` | Основной код загрузчика |

---

## Сборка

```bash
nasm -f elf32 bootloader.s -o bootloader.o
