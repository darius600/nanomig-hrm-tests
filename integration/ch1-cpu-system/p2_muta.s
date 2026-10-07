; p2_muta.s -- p2.s with MUT_A set: P2.a's contrast (SPECIFICATION.md
; section 4): step 8's S_B write is made twice, so the window-close marker
; is not unique while the window (first S_A to first S_B) stays usable.
MUT_A           equ     1
        include "p2.s"
