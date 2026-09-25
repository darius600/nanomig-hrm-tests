; prog_sharing_tiny.s -- A2 mutation: prog_sharing.s with SHARE and TINY
; both set. BLTPRI stays clear (share mode) but the blit shrinks to a
; near-instant one-word transfer, so the active window it opens is far too
; short to contain several CPU-loop iterations.

SHARE   equ     1
TINY    equ     1
        include "prog_sharing.s"
