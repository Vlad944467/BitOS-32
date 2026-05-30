ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text : {
        *(.multiboot)
        . = ALIGN(4096);
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /* Отмечаем конец ядра (для отладки) */
    kernel_end = .;
}
