; p6_mute.s -- p6.s with MUT_E set: P6.e's contrast (SPECIFICATION.md
; section 8): every guard shortened to a single word, still never
; executed, so D(68000 mode) equals the guard length instead of being
; less than it.
MUT_E           equ     1
        include "p6.s"
