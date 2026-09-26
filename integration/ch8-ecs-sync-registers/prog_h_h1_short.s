; prog_h_h1_short.s -- X-SHORT mutation: H1 (N=$00B0, B=$08A0) with the
; sampling loop shortened to 16 blocks (1024 samples) instead of 256
; (16384). Must fail P1-LINES and P2-LINES (too few lines/wraps covered,
; and the run also finishes sooner, shortening the MARK1..MARK2 window
; P2's refresh-period count depends on).
N               equ     $00B0
B               equ     $08A0
SHORT_LOOP      equ     1
        include "prog_h.s"
