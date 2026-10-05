; prog_p7_g3.s -- P7-G3's mutation (stimulus red): the BLTSIZE write is
; replaced by a COLOR00 write, so no blit starts.

G3_NOBLIT       equ     1
        include "prog_p7.s"
