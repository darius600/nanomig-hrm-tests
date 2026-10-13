; prog_h_h2_short.s -- X-SHORT mutation for H2 (N=$00C0, B=$08A0): the sampling
; loop shortened to 16 blocks (1024 samples) instead of 256 (16384), followed
; by a delay loop that keeps the MARK1..MARK2 window as long as the normal
; run's. Must fail P1-LINES [H2] (too few wraps) and nothing else.
N               equ     $00C0
B               equ     $08A0
SHORT_LOOP      equ     1
HOLD_N          equ     24000
        include "prog_h.s"
