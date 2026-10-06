; prog_beam_o_nolace.s -- prog_beam_o.s with MUT_NOLACE: BPLCON0 = $0200 in step 1
SWEEP           equ     1
MUT_NOLACE      equ     1
        include "prog_beam.s"
