; prog_fill_asc.s -- carries the page 182/184/200 descending-mode-required
; ids: prog_fill_desc.s with ASCENDING defined, so the identical blit runs
; with the pointer started at word0 and DESC cleared instead of set.

ASCENDING equ 1
        include "prog_fill_desc.s"
