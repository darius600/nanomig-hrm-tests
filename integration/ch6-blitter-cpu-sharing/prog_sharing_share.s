; prog_sharing_share.s -- prog_sharing.s with SHARE set: BLTPRI stays
; clear, so the blitter and 68000 share the bus per the three-
; consecutive-cycle rule.

SHARE   equ     1
        include "prog_sharing.s"
