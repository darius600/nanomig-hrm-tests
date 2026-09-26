; prog_fastrun_long_chipcode.s -- prog_fastrun.s with FR_LONG and
; FR_CHIPCODE both set: FR_ITERS=2000 (prog_fastrun_long.s) and FR_CODE in
; chip RAM (prog_fastrun_chipcode.s), together -- FASTRUN's own mutation
; on the 2000-iteration build. See prog_fastrun.s for the whole design.

FR_LONG     equ     1
FR_CHIPCODE equ     1
        include "prog_fastrun.s"
