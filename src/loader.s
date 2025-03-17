global loader                   ; the entry symbol for ELF
extern kmain

; Multiboot header constants
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002  ; magic number for Multiboot
MULTIBOOT_HEADER_FLAGS equ 0x00000003  ; flags: align modules on page boundaries
                                       ; and provide memory map
MULTIBOOT_HEADER_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes

section .multiboot              ; Multiboot header section
align 4                         ; align at 4 bytes
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC   ; magic number
    dd MULTIBOOT_HEADER_FLAGS   ; flags
    dd MULTIBOOT_HEADER_CHECKSUM ; checksum

section .bss
align 4				; align at 4 bytes
kernel_stack:			; label points to beginning of memory
    resb KERNEL_STACK_SIZE	; reserve stack for the kernel

section .text:                  ; start of the text (code) section
loader:                         ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                ; stack (end of memory area)
    call kmain
    hlt
