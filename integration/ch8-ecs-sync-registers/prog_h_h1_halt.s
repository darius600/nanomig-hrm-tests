; prog_h_h1_halt.s -- X-HALT-H mutation: H1 (N=$00B0, B=$08A0) with
; BRA.S * inserted immediately before the HTOTAL write. Must fail PC-MARK:
; neither MARK1 nor MARK2 is ever written.
N               equ     $00B0
B               equ     $08A0
HALT_BEFORE     equ     1
        include "prog_h.s"
