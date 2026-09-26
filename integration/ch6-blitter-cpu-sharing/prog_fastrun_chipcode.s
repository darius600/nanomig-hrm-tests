; prog_fastrun_chipcode.s -- prog_fastrun.s with FR_CHIPCODE set: FR_CODE
; moves to chip RAM ($0C8000); FR_CNT stays in fast RAM. This program's own
; MUTATION (NMH_MUTATE=FASTRUN, in place of prog_fastrun.bin); see
; prog_fastrun.s's header for the whole design and what this substitution
; predicts.

FR_CHIPCODE equ     1
        include "prog_fastrun.s"
