; MUTATION support: P1-c contrast, config C -- DMACON = $8280 (BPLEN not set);
; no bitplane reads occur.
PALETTE16       equ     1
HIRES_MODE      equ     1
MUT_NOBPLEN     equ     1
        include "prog_playfield_core.s"
