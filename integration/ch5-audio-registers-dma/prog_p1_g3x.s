; prog_p1_g3x.s -- P1-G3's cross-window mutation (stimulus red): one extra
; CPU write into A_0 (the block under test for channel 0), through an
; absolute address, inside window W_1.

G3X_BREAK       equ     1
        include "prog_p1.s"
