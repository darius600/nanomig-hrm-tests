; prog_p11_g3q.s -- P11-G3's second mutation (stimulus red): one extra CPU
; write of $1234 into Q2, inside the window. A blitter write there carries
; $FFFF (BLTCON0 $01FF), so the write is not a blitter write.

G3Q_BREAK       equ     1
        include "prog_p11.s"
