; prog_p7_lf0.s -- P7 contrast: BLTCON0 holds LF = $00, so the blit writes $0000 instead of $FFFF.

P7_LF0          equ     1
        include "prog_p7.s"
