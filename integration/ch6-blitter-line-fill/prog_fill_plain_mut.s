; prog_fill_plain_mut.s -- MUTATION support for FIXTURE-FILL: prog_fill_plain.s
; with MUTATE_MARKER defined, dropping the word4 marker, so the exact
; five-word reproduction the fixture checks for must go red.

MUTATE_MARKER equ 1
        include "prog_fill_plain.s"
