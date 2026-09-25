; prog_sharing_nasty.s -- prog_sharing.s with NASTY set: BLTPRI is set,
; so the blitter never yields the bus.

NASTY   equ     1
        include "prog_sharing.s"
