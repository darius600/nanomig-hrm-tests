; prog_p8_shortblit.s -- clause (b) of P8 (stimulus red): BLTSIZE $1020 (64 rows of 32
; words) in place of $2020, so the blit ends earlier.

BSIZE   equ     $1020
        include "prog_p8.s"
