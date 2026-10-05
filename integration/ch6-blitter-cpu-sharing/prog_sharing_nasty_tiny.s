; prog_sharing_nasty_tiny.s -- A4PRE mutation: prog_sharing.s with NASTY
; and TINY both set. BLTPRI is set (nasty mode) but the blit shrinks to a
; near-instant one-word transfer, so the active window it opens is far too
; short to contain several CPU-loop iterations.

NASTY   equ     1
TINY    equ     1
        include "prog_sharing.s"
