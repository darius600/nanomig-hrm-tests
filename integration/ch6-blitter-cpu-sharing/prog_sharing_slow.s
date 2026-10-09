; prog_sharing_slow.s -- A4-SHARE mutation: prog_sharing.s with SLOW set. The
; BASELINE counting loop with two register-only MULU.W instructions between
; its Chip-memory accesses, so that its access rate with the bus to itself is
; below the rate of the share-mode window's.

SLOW    equ     1
        include "prog_sharing.s"
