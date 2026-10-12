; prog_lpen_m4_tg.s -- prog_lpen.s with the V8 and V9 bits of the vertical
; counter set (VPOSW) for the reads of the measurement loop that lie before
; the pulse of F4, on the TG68K core (the beam is at line 22 of that field at
; the 14350th read and at line 101 of it 900 reads later).
M_AT    equ 14350
M_LEN   equ 900
M_ON    equ $8003
        include "prog_lpen.s"
