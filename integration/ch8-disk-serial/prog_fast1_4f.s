; Precondition 4f contrast: FAST=1, with word 100 of the buffer overwritten with
; $5555 after the fill, so the buffer no longer holds $AAAA throughout.
MUT_FAST1       equ 1
MUT_BADFILL     equ 1
BND_POLL        equ 200000
        include "fast_core.s"
