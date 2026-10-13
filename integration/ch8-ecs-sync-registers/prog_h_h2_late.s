; prog_h_h2_late.s -- X-LATE mutation for H2 (N=$00C0, B=$08A0): the full 16384
; samples, but MARK1 is written only after the last of them, followed by a
; short delay loop, so the MARK1..MARK2 window holds far fewer than 320 line
; periods. Must fail P2-LINES [H2] and nothing else.
N               equ     $00C0
B               equ     $08A0
LATE_MARK       equ     1
HOLD_N          equ     3000
        include "prog_h.s"
