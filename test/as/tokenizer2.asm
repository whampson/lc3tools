LABEL1                      ; label by itself
RTI                         ; instr by itself
.END                        ; macro by itself

JMP R6                      ; instr w/ one operand (reg)
BR LABEL1                   ; instr w/ one operand (label)
TRAP x25                    ; instr w/ one operand (imm)
.FILL LABEL1                ; macro w/ one operand (label)
.FILL xBEEF                 ; macro w/ one operand (imm)

ADD R0, R1, R2              ; instr w/ many operands (reg)
ADD R3, R4, #3              ; instr w/ many operands (positive imm)
ADD R3, R4, #-6             ; instr w/ many operands (negative imm)
ADD R5, R6, xE              ; instr w/ many operands (hex)
ADD R4,R5,#3                ; instr w/ minimal spacing & positive imm
    LEA R7, LABEL           ; instr w/ leading space and label operand
ADD R4,   R5,       #14      ; instr w/ weird spacing (extra whitespace after ,)
ADD   R0   , R5  ,  xA      ; instr w/ weird spacing (extra whitespace before ,)
ADD   R7   , R0  ,R6        ; instr w/ weird spacing (whitespace mixture)
ADD   R4   ,R2  , R1        ; instr w/ weird spacing (whitespace mixture)
	ADD	R7,	R6,		R5      ; instr w/ weird spacing (tabs)
