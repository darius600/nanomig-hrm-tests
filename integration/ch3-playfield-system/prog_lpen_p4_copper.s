; MUTATION support: P4-b contrast, Copper part -- before the main loop, a
; Copper list (preloaded by the test at COP_LIST_ADDR: MOVE BPLCON0,$0008 /
; WAIT $FFFF,$FFFE) is started, so a MOVE to $100 is fetched at the
; Chip-memory port even though the program itself never writes BPLCON0.
OMIT_BPLCON0       equ     1
MUT_COPPER_BPLCON0 equ     1
        include "prog_lpen_core.s"
