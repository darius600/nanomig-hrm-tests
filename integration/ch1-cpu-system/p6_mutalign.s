; p6_mutalign.s -- p6.s with MUT_ALIGN set: the new alignment precondition's
; contrast (SPECIFICATION.md section 8): the first pair's extra pad word is
; left out, so X1 and X2 land at the same parity mod 4 instead of differing.
MUT_ALIGN       equ     1
        include "p6.s"
