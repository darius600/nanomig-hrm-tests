; prog_r3_nocfg.s -- R3u (section 5.2.6 contrast for IMG2!=IMG0): steps c
; (the 48 write) and d are skipped; IMG2 is read as the last action after
; only steps a and b, so the board is never configured.

MUT_R3U equ 1
        include "prog_r3.s"
