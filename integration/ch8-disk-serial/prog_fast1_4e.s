; Precondition 4e contrast: FAST=1, but LENGTH=128 ($C080, written twice)
; instead of 256 -- the fetches end at i=127.
MUT_FAST1       equ 1
MUT_LEN128      equ 1
BND_POLL        equ 200000
        include "fast_core.s"
