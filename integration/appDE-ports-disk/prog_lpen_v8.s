; prog_lpen_v8.s -- prog_lpen.s with the V8 bit of the vertical counter
; flipped (VPOSW) for 100 reads inside the reference phase.
W_V8    equ 1
W_AT    equ 3000
W_LEN   equ 100
        include "prog_lpen.s"
