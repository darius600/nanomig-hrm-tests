; Precondition 4e contrast, FAST=1 run: after the transfer the CPU reads one buffer word
; (word 10) that the disk DMA already fetched, so it is read twice.
MUT_FAST1       equ 1
MUT_ONCE        equ 1
BND_POLL        equ 200000
        include "fast_core.s"
