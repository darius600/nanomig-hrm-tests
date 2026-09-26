; prog_h_h1_burst.s -- X-BURST mutation: H1 (N=$00B0, B=$08A0) whose
; sampling loop reads VHPOSR without storing until H decreases (a wrap),
; then stores 4 samples, repeated 4096 times (16384 stores total, same
; count as the normal loop). Must fail P1-COVER: samples cluster right
; after each wrap, leaving a whole line uncovered between one burst's
; last store and the next burst's first.
N               equ     $00B0
B               equ     $08A0
BURST_MODE      equ     1
        include "prog_h.s"
