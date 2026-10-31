; prog_lpen_late.s -- prog_lpen.s with the first read LATE dbra iterations after the
; marker: 89 (TG68K board kernel), 133 (TG68K shipped kernel) or 111 (fx68k) line
; boundaries between the marker and the store of w1.
LATE    equ     4994
        include "prog_lpen.s"
