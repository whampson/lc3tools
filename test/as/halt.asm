.ORIGIN 0x3000

MAIN:
    AND     R3, R3, R3
    ADD     R3, R3, 1

HALT:
    ; Set up mask
    AND     R5, R5, 0
    ADD     R5, R5, 1
    LSHF    R5, R5, 15
    NOT     R5, R5

    ; Load MCR
    LEA     R6, MCR
    LDI     R7, R6, 0

    ; Turn off clock enable bit
    AND     R7, R7, R5
    STI     R7, R6, 0

_HALT_LOOP:
    BR _HALT_LOOP

CLOCK_EN:   .FILL   0x8000
MCR:        .FILL   0xFFFE
