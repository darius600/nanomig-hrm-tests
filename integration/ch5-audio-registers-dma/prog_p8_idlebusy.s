; prog_p8_idlebusy.s -- clause (a) of P8 (stimulus red): a blit of 128 words to $76000 runs
; across the four idle reads and has ended before the main blit starts.

IDLE_BUSY        equ     1
        include "prog_p8.s"
