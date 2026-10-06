; prog_lpen_m8.s -- prog_lpen.s with the V8 bit of the vertical counter
; flipped (VPOSW) for 100 reads of the measurement loop, inside the field
; that carries the F2 pulse and before that pulse.
M_AT    equ 4600
M_LEN   equ 100
        include "prog_lpen.s"
