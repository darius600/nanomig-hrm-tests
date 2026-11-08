; prog_fill_asc_mut.s -- MUTATION support for the ascending assertion
; (leading id 6-descending-mode.c10): the opposite case of the clause
; "area fill only works correctly in descending mode" (p.200). It is
; prog_fill_desc.s unchanged -- the same three markers, DESC = 1, pointers
; at word4 -- placed in the ascending slot. The fill is the correct one
; (word1 = 0000, word3 = FFFF), so the ascending assertion must turn red,
; while the destination was written (PRECONDITION-FILL-ASC stays green).

        include "prog_fill_desc.s"
