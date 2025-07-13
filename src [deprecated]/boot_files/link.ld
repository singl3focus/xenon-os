ENTRY(loader)

SECTIONS {
    . = 0x00100000;

    .multiboot ALIGN(8) : {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata*)
    }

    .data ALIGN(0x1000) : {
        *(.data)
    }

    .bss ALIGN(0x1000) : {
        *(COMMON)
        *(.bss)
    }
}