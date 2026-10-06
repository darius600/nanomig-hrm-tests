; Precondition 4c contrast, FAST=0 run: ADKCON <- $8100 (bit 15 set) SETS bit 8,
; so ADKCONR must read back 1, not 0.
MUT_SETFAST0    equ 1
BND_POLL        equ 200000
        include "fast_core.s"
