; prog_p8_g4.s -- P8-G4's mutation (stimulus red): a delay between the
; BLTSIZE write and the first poll read, so the blit has ended before any
; read is made and no read i >= 2 falls before T_last.

G4_LATE         equ     1
        include "prog_p8.s"
