; scratch area for testing tokenizer

RET
JMP R0
NOT R7,R6
AND R3, R2, R1
ADD R4 ,R5 ,R6
XOR R7 , R0 , R1

:
.
,
_

label: label2:
another_label    :

"foo" "bar" 13 0
"W@cky ch@r$ ~!@#$%^&*()_+`[]{}|;':,./<>?"
"Escape sequences\\\"\r\nfoo\tbar"
0xff 1 -1 -0x45 010 ; yes the last one is octal :D

.FILL

; all of the following should cause a syntax error
; -
; -abc
; +
; $
; @
; "
; "unterminated
; "shared"quote"
; "bad \escape"
; 5inner
; b@d_label: