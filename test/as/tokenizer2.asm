LABEL1                      ; label by itself
RTI                         ; instr by itself

JMP R6                      ; instr w/ one operand (reg)
BR LABEL1                   ; instr w/ one operand (label)
TRAP x25                    ; instr w/ one operand (imm)

ADD R3, R2, R1              ; instr w/ many operands (reg)
ADD R3, R2, R1              ; instr w/ many operands (reg)
ADD R4,R5,#3                ; instr w/ minimal spacing & positive imm
    ADD R4,   R5,       #-3 ; instr w/ weird spacing & negative imm
ADD   R0   , R5  ,  xE      ; instr w/ weird spacing & hex imm
ADD   R7   , R0  ,R6        ; instr w/ weird spacing
	ADD	R7,	R6,		R5      ; instr w/ weird spacing (tabs)
