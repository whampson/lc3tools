; this is a comment (consider // as well)
      ; this is a comment with leading whitespace!
jmp r0

label1 add r0,r1,r3
label2 xor r4,r5,#6
label3 and r6,r7,x1C ; with a trailing comment!

label4  ; now we've got a macro and newline!
    .fill xA5A5
    .blkw 50
    .stringz "Hello, world!"


label               ; address is 0x46 (I think... & if program above doesn't change)

ADD R5, R5, R5      ; 0001 1011 0100 0101   = 0x1B45
ADD R5, R5, #-1     ; 0001 1011 0111 1111   = 0x1B7F

AND reg, reg, R5    ; 0101 1011 0100 0101   = 0x5B45
AND reg, reg, x1F   ; 0101 1011 0111 1111   = 0x5B7F

BR label            ; 0000 111 000000000    =
BRn label
BRz label
BRp label
BRnz label
BRnp label
BRzp label
BRnzp label     ; same as BR (unconditional branch)

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

; expand on these (HALT, PUTS, etc...)
TRAP x25

XOR R5, R5, R5
XOR R5, R5, x1F