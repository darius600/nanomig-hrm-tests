; MUTATION support: P2 item-3 contrast -- the display window is 32 pixels
; wider and one more word per line is fetched (BPL1MOD -2 keeps the line
; pitch), so the sample after the 320th pixel shows bitplane data.
MUT_RIGHTEDGE   equ     1
        include "prog_playfield_core.s"
