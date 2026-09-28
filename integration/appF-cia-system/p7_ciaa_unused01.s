; P7.7, CIAA, baseline run ($01). See p_unused_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $01
                include "p_unused_body.s"
