; prog_p1_outlow.s -- prog_p1.s with OUTLOW set: the Fc write for C1-C4
; sets the output-enable bit's data bit to 0 instead of 1 ($0201, $0801,
; $2001, $8001), keeping the output-enable bit and START set. See
; SPECIFICATION.md section 6, the C1-C4-Fc-OUT contrasting case.

OUTLOW  equ     1
        include "prog_p1.s"
