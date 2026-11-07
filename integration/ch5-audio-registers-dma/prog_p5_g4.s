; prog_p5_g4.s -- P5-G4's mutation (stimulus red): the delay is 300 spin iterations, long
; enough for the 16 x 16 blit to write its 256 words and short of a frame, so only V(W) falls short.

DELAY_N equ     300
        include "prog_p5.s"
