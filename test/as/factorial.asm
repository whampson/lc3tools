;===============================================================================
;   FILE: factorial.asm
; AUTHOR: Wes Hampson (whampso2)
;
; DESCRIPTION:
; Iteratively computes n! (n factorial), where n is an integer.
; Written for ECE411 MP0, Spring 2018.
;
; REGISTER DETAIL:
;   R0: (constant)  0, used as a base address register
;   R1: (constant) -1, used for decrementing loop counters
;   R2: (constant) -2, used for setting up initial loop counter value
;   R3: accumulator, contains the result of each addition performed
;   R4: multiplicand, the number currently being multiplied
;   R5: factorial loop counter (outer loop)
;   R6: multiply loop counter (inner loop) (i.e., multiplier)
;   R7: (unused)
;
; ASSUMPTIONS:
;   * R0 contains a value of 0 upon entry.
;===============================================================================

ORIGIN 4x0000

; SEGMENT CodeSegment:                  ; TODO: support segments?
INIT:
    ; Load constants used throughout the program.
    LDR R1, R0, NEG_ONE
    LDR R2, R0, NEG_TWO

    ; Initialize accumulator and multiplicand.
    LDR R3, R0, INPUT	; sum = input, accumulator
    LDR R4, R0, INPUT   ; amt = input, multiplicand (amount added per iteration)

    ; Handle case for 0!
    BRz ZERO_INPUT

    ; Initialize loop counters
    ADD R5, R3, R2      ; i = input - 2, subtract 2 because 'sum' initialized to 'input'
    ADD R6, R0, R5      ; k = i

MULTIPLY_LOOP:
	; Computes 'amt*k' and adds the result to 'sum'
    BRnz STORE
    ADD R3, R3, R4      ; sum = sum + amt
    ADD R6, R6, R1      ; k = k - 1
    BRnzp MULTIPLY_LOOP

STORE:
    STR R3, R0, RESULT  ; result = sum

NEXT_FACTORIAL:
    ; Decrement loop counters and update multiplcand
    LDR R4, R0, RESULT  ; amt = sum
    ADD R5, R5, R1      ; i = i - 1
    BRnz HALT           ; if i < 1; halt
    ADD R6, R0, R5      ; k = i
    nop
    BRnzp MULTIPLY_LOOP

ZERO_INPUT:
    ; Special case for 0!, stores 1 in the result.
    LDR R3, R0, POS_ONE
    STR R3, R0, RESULT

HALT:
    BRnzp HALT


; Data input/output
INPUT:      DATA2   5
RESULT:     DATA2   0

; Constants
POS_ONE:    DATA2   1
NEG_ONE:    DATA2  -1
NEG_TWO:    DATA2  -2
