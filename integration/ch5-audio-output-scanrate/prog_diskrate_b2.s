; prog_diskrate_b2.s -- prog_diskrate.s with MUT_B2 set: precondition
; B2's own contrasting case. Built code: run F's DMACON write becomes
; $8010 instead of $8210 (DMAEN, bit 9, cleared; DSKEN, bit 4, stays set),
; so DMAON reads 0 (p.247 step 1, p.248, p.278).
MUT_B2  equ     1
        include "prog_diskrate.s"
