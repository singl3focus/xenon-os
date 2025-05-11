ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    . = 0xC0100000;          /* Виртуальный адрес: 3 ГБ + 1 МБ */

    .multiboot ALIGN (0x1000) : AT(ADDR(.multiboot) - 0xC0000000) {
        *(.multiboot)
    }

    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000) {
        *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        *(.data)
        /* Явное включение таблиц */
        . = ALIGN(4K);
        pdt = .;
        *(.pdt)
        gdt_start = .;
        *(.gdt)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
        kernel_stack = . + 4096; /* Стек после секции .bss */
    }
}
