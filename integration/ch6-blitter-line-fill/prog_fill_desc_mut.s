; prog_fill_desc_mut.s -- MUTATION support for FILL-DESC-CORRECT:
; prog_fill_desc.s with MOVE_MARKER defined, moving the middle marker from
; word2 to word1. Word1 is checked as a marker-free interior word by
; FILL-DESC-CORRECT, so this must turn that check red.

MOVE_MARKER equ 1
        include "prog_fill_desc.s"
