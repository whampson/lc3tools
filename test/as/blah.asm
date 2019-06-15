; this is a comment (consider // as well)
jmp r0

label1 add r0,r1,r3
label2 xor r4,r5,#6
label3 and r6,r7,x1C ; this is a trailing comment!

label4
    .fill xA5A5
    .blkw 50
    .stringz "Hello, world!"


label               ; address is 0x46 (I think... & if program above doesn't change)

ADD R5, R5, R5      ; 0001 1011 0100 0101   = 0x1B45
ADD R5, R5, #-1     ; 0001 1011 0111 1111   = 0x1B7F

AND R5, R5, R5      ; 0101 1011 0100 0101   = 0x5B45
AND R5, R5, x1F     ; 0101 1011 0111 1111   = 0x5B7F

BR label            ; 0000 111 000000000    =
BRn label
BRz label
BRp label
BRnz label
BRnp label
BRzp label
BRnzp label

JMP R5

JSR label
JSRR R5

LDB R5, R5, label
LDW R5, R5, label
LDI R5, R5, label

LEA R5, label

NOT R5, R5

RET
RTI

LSHF R5, R5, #15
RSHFL R5, R5, xF
RSHFA R5, R5, R5

STB R5, R5, label
STW R5, R5, label
STI R5, R5, label

TRAP x08

XOR R5, R5, R5
XOR R5, R5, x1F

GETC        ; TRAP x20
OUT         ; TRAP x21
PUTS        ; TRAP x22
IN          ; TRAP x23
PUTSP       ; TRAP x24
HALT        ; TRAP x25
