; P7.7, CIAB, baseline run ($01). See p_unused_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $01
                include "p_unused_body.s"
