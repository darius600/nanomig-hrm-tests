; prog_h_h1c_short.s -- X-SHORT mutation for H1c (N=$00B0, B=$0820): the sampling
; loop shortened to 16 blocks (1024 samples) instead of 256 (16384), followed
; by a delay loop that keeps the MARK1..MARK2 window as long as the normal
; run's. Must fail P1-LINES [H1c] (too few wraps) and nothing else.
N               equ     $00B0
B               equ     $0820
SHORT_LOOP      equ     1
HOLD_N          equ     24000
        include "prog_h.s"
