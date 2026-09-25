; prog_fill_asc_mut.s -- MUTATION support for the ascending assertion
; (leading id 6-descending-mode.c10): prog_fill_desc.s with ASCENDING and
; DROP_MARKER0 both defined. Removing
; word0's marker leaves only word2 and word4 as boundary crossings, so the
; ascending scan (still the wrong mode) coincidentally reproduces the
; descending-mode word1=0000/word3=FFFF pattern -- the mismatch check must
; turn red even though the mode is still ascending.

ASCENDING    equ 1
DROP_MARKER0 equ 1
        include "prog_fill_desc.s"
