; prog_p2_off.s -- P2 contrasting case, graded in the normal run: phase 2
; writes DMACON = $0000 (no-op). See prog_p2_base.s.
P2_MAIN_OFF     equ     1
        include "prog_p2_base.s"
