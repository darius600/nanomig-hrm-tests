; Precondition 4f contrast: FAST=0, with word 100 of the buffer overwritten with
; $5555 after the fill, so the buffer no longer holds $AAAA throughout.
MUT_BADFILL     equ 1
BND_POLL        equ 200000
        include "fast_core.s"
