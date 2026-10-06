; Precondition 4d contrast, FAST=0 run: partway through the transfer DSKEN
; (DMACON bit 4) is cleared for a while and then reasserted (see prog_fast1_4d.s).
MUT_MIDSTOP4D   equ 1
MIDSTOP4D_DELAY equ 199819
PAUSE4D         equ 40
BND_POLL        equ 200000
        include "fast_core.s"
