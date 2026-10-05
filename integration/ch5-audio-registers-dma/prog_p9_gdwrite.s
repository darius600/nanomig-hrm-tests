; prog_p9_gdwrite.s -- P9-G(reads)'s mutation (stimulus red): blit 4 also
; uses D, so it writes outside the CPU's own areas.

D4_ON           equ     1
        include "prog_p9.s"
