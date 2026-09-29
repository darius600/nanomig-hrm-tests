; prog_p6_g3.s -- P6-G3's mutation (stimulus red): one extra CPU write
; into S0 (the block under test), past its own content, inside the window.

G3_BREAK        equ     1
        include "prog_p6.s"
