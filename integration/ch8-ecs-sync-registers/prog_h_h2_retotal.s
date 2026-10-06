; prog_h_h2_retotal.s -- P2A-H2 mutation: H2 (N=$00C0, B=$08A0) with HTOTAL
; written again mid-loop (RETOTAL_MID), changing it to $00D0 after 128 of
; the sampling loop's 256 blocks. p.280 states no exception for a second
; write, so the graded periods after this point are page-predicted to
; differ from those before it, breaking P2(a) [H2]'s "one and the same
; value".
N               equ     $00C0
B               equ     $08A0
RETOTAL_MID     equ     1
RETOTAL_AT      equ     127
RETOTAL_N       equ     $00D0
        include "prog_h.s"
