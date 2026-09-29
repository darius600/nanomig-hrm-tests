; prog_p5_mut_hhi.s -- P5-H-high's own mutation (NMH_MUTATE=P5-HHI): one
; extra phase-1 step writes a wrong H value where S_H1's own A19 entry
; ($08F000) is graded, leaving every A16-A18 entry as the main run
; prescribes. See prog_p5.s.

P5_MUT_HHI      equ     1
        include "prog_p5.s"
