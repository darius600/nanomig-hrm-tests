; prog_fill_asc.s -- FILL-ASC-INCORRECT: prog_fill_desc.s with ASCENDING
; defined, so the identical blit runs with the pointer started at word0
; and DESC cleared instead of set.

ASCENDING equ 1
        include "prog_fill_desc.s"
