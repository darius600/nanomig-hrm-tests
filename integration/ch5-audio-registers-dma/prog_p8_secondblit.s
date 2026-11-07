; prog_p8_secondblit.s -- clause (c) of P8 (stimulus red): a second blit (BLTSIZE $0820) is
; started after the poll has stopped and before the 8 extra reads.

SECOND_BLIT      equ     1
        include "prog_p8.s"
