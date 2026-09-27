; Precondition 4c contrast: intends FAST=1 but writes ADKCON <- $0100
; (bit 15 clear), which CLEARS bit 8 instead of setting it -- ADKCONR must
; read back 0, not 1.
MUT_FAST1       equ 1
MUT_4C          equ 1
BND_POLL        equ 200000
        include "fast_core.s"
