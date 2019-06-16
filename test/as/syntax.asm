;
; Syntax test.
; Will probably cause the CPU to run off into the weeds if executed :D
;

START:                      ; label by itself
BEGIN:  RET                 ; label w/ opcode, no operands

        ; operand types
        JMP R7              ; register
        BR START            ; address (label)
        BRnz -1             ; address (literal, dec)
        TRAP 0x25           ; address (literal, hex)
        NOT R0, R6          ; register & register
        LEA R0, DATA0       ; register & address (as label)
        LEA R1, 60          ; register & address (literal, dec)
        LEA R2, 0x3C        ; register & address (literal, hex)
        LDW R3, R5, DATA1   ; register & register & address (label)
        STW R3, R5, 2       ; register & register & address (literal)
        ADD R0, R1, R2      ; register & register & register
        ADD R3, R4, -2      ; register & register & immediate (dec)
        ADD R5, R6, 1E      ; register & register & immediate (hex)

        ; weird spacing
        ADD R0   , R5  ,  0xA   ; extra whitespace before ,
        ADD R7   , R0  ,R6      ; whitespace mixture
        ADD R4   ,R2  , R1      ; whitespace mixture
		ADD	R7	,	R6,		R5  ; tabs

DATA0:  .FILL 0xBEEF        ; pseudo-op w/ hex operand
DATA1:  .FILL 1337          ; pseudo-op w/ dec operand
HELLO:  .ASCII "Hello, world!", 13, 0

FOO:
