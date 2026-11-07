; prog_p1_join.s -- the join contrast of P1 (stimulus red): channel 0's enabling write leaves
; DMAEN out (its instance is NOT GRADED) and channel 1's location is A_1^H (its instance fails).

G2_BREAK        equ     1
LOC1    equ     A1H
        include "prog_p1.s"
