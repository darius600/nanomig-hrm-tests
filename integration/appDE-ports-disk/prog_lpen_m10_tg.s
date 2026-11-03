; prog_lpen_m10_tg.s -- prog_lpen.s with the V8 and V9 bits of the vertical
; counter set (VPOSW) for 100 reads of the measurement loop, inside F10 and
; before its line 100, on the TG68K core (the reads of F10 before line 100 are
; the 37901st to the 38792nd of the loop).
M_AT    equ 38300
M_LEN   equ 100
M_ON    equ $8003
        include "prog_lpen.s"
