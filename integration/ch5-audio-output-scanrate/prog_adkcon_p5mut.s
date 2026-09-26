; prog_adkcon_p5mut.s -- prog_adkcon.s with MUT_P5 set: P5's own
; contrasting case (step 2 writes $8200 instead of $8100).
MUT_P5  equ     1
        include "prog_adkcon.s"
