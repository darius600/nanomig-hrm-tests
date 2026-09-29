; prog_p8_blit_nosize.s -- P8 BLIT case, device contrast: omit the BLTSIZE
; write. Bit 6 stays 0. See prog_p8_blit_base.s.
P8_BLIT_NOSIZE  equ     1
        include "prog_p8_blit_base.s"
