; prog_diskrate_b2.s -- prog_diskrate.s with MUT_B2 set: precondition
; B2's own contrasting case (run F's second, step-6 DSKLEN write is left
; out; p.247: "the DMAEN bit in the DSKLEN register must be turned on
; twice in order to actually enable the disk DMA hardware").
MUT_B2  equ     1
        include "prog_diskrate.s"
