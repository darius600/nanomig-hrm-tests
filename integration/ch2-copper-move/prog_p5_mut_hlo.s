; prog_p5_mut_hlo.s -- P5-H-low's own mutation (NMH_MUTATE=P5-HLO): one
; extra phase-1 step writes a wrong H value where S_H1's own A18 entry
; ($04F000) is graded, leaving every A19/A20 entry as the main run
; prescribes. See prog_p5.s.

P5_MUT_HLO      equ     1
        include "prog_p5.s"
