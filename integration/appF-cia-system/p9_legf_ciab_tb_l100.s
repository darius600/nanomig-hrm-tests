; p9_legf_ciab_tb_l100.s -- P9 leg F, CIAB/timer B, L=100. Reuses p3_body.s (P3's own
; mechanism: toggle mode, R-SAMPLE(600;PRB,ICR)).
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     100
CRVAL           equ     $07
                include "p3_body.s"
