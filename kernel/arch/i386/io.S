.global outb
.global inb
.global outw
.global inw

/* outb - send a byte to an I/O port
   stack: 4(%esp) - I/O port
          8(%esp) - data byte */
outb:
    movb    8(%esp), %al     # Move data byte to AL
    movw    4(%esp), %dx     # Move I/O port to DX
    outb    %al, %dx         # Output byte to port
    ret

/* inb - returns a byte from given I/O port
   stack: 4(%esp) - I/O port */
inb:
    movw    4(%esp), %dx     # Move I/O port to DX
    inb     %dx, %al         # Input byte from port to AL
    ret                      # Result in AL

/* outw - send a word to an I/O port
   stack: 4(%esp) - I/O port
          8(%esp) - data word */
outw:
    movw    4(%esp), %dx     # Move I/O port to DX
    movw    8(%esp), %ax     # Move data word to AX
    outw    %ax, %dx         # Output word to port
    ret

/* inw - returns a word from given I/O port
   stack: 4(%esp) - I/O port */
inw:
    movw    4(%esp), %dx     # Move I/O port to DX
    inw     %dx, %ax         # Input word from port to AX
    ret                      # Result in AX
    