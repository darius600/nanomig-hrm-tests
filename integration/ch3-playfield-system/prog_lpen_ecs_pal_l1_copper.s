; MUTATION support: P3-d contrast, clause on reads outside the program image -- the
; L1 (ECS, PAL) program points COP1LC at the list preloaded at COP_LIST_ADDR and starts
; the Copper before the main loop. The test preloads a list that holds only the WAIT
; $FFFF,$FFFE, so the Copper fetches at the port and writes no register.
ECS_MODE           equ     1
LPEN1              equ     1
MUT_COPPER_BPLCON0 equ     1
        include "prog_lpen_core.s"
