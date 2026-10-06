; prog_p8_zpoll.s -- P8's own mutation (stimulus red): the blit still
; runs, and the fourth poll read comes from a zero word (Zb) instead of
; DMACONR, so a graded read made while the blit is in progress shows
; bit 14 = 0. A delay at the end lets the blit finish before the program
; does, so that its last write is logged.

ZPOLL           equ     1
        include "prog_p8.s"
