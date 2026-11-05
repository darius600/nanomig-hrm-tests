; prog_diskrate_b2.s -- prog_diskrate.s with MUT_B2 set: precondition
; B2's own contrasting case. Run F transfers all 544 words as in the normal
; run; DMACON $0200 (DMAEN, bit 9, cleared; DSKEN, bit 4, stays set) is
; then written and DSKBYTR read, so DMAON reads 0 (p.247 step 1, p.248,
; p.278) with the buffer reads of B3 and the fixture lines intact.
MUT_B2  equ     1
        include "prog_diskrate.s"
