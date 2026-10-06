; prog_p9_gwrites.s -- P9-G(writes)'s mutation (stimulus red): blit 1 has
; BLTSIZE $0103 (height 4, width 3: 12 words) instead of $0104, so it makes
; 12 writes of $0000, not 16. Its logic output is still zero, so bit 13 of
; Z_1 is unchanged.

BSIZE1          equ     $0103
        include "prog_p9.s"
