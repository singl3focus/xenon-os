global outb
global inb
global outw
global inw

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address

outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function

; inb - returns a byte from the given I/O port
; stack: [esp + 4] The address of the I/O port
;        [esp    ] The return address

inb:
    mov dx, [esp + 4]    ; move the address of the I/O port to the dx register
    in  al, dx           ; read a byte from the I/O port and store it in the al register
    ret                  ; return the read byte

global load_idt

load_idt:
    mov eax, [esp+4]
    lidt [eax]
    ret

outw:
    mov dx, [esp + 4]
    mov ax, [esp + 8]
    out dx, ax
    ret

inw:
    mov dx, [esp + 4]
    in ax, dx
    ret