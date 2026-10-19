; prog_p11_g3f.s -- P11-G3's third mutation (stimulus red): one extra CPU
; write of $FFFF into Q2, inside the window. The data is the one a blitter
; write there carries (BLTCON0 $01FF), so only the requester differs.

G3F_BREAK       equ     1
        include "prog_p11.s"
