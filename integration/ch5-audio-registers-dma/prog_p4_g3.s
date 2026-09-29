; prog_p4_g3.s -- P4-G3's mutation (stimulus red): one extra CPU write
; into L (the block under test), past its own content, inside the window.

G3_BREAK        equ     1
        include "prog_p4.s"
