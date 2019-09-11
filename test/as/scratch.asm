RET
JMP R0
NOT R7,R6
AND R3, R2, R1
ADD R4 ,R5 ,R6
XOR R7 , R0 , R1

; valid tokens but not syntactically correct
"foo" "bar" 13 0
0xff 1 -1 -3434
R0 R1 R7 abc

;"unterminated       ; should fail

; comment
label_ref
label_dec:

.MACRO

;
;:      ; should fail
;.      ; should fail
,
;-      ; should fail
_
;+      ; should fail
;$      ; should fail
a
0
;5inner ; should fail
;b@d_label: ; should fail
boz0