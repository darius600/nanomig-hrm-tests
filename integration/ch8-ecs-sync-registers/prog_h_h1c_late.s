; prog_h_h1c_late.s -- X-LATE mutation for H1c (N=$00B0, B=$0820): the full 16384
; samples, but MARK1 is written only after the last of them, followed by a
; short delay loop, so the MARK1..MARK2 window holds far fewer than 320 line
; periods. Must fail P2-LINES [H1c] and nothing else.
N               equ     $00B0
B               equ     $0820
LATE_MARK       equ     1
HOLD_N          equ     3000
        include "prog_h.s"
