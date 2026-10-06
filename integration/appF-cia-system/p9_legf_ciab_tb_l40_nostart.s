; p9_legf_ciab_tb_l40_nostart.s -- P9 leg F, CIAB/timer B, L=40, with the timer not started (CRVAL=$06): no
; flag appears. Reuses p3_body.s (P3's own mechanism: toggle mode, R-SAMPLE(600;PRB,ICR)).
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
M               equ     $80
LATCHVAL        equ     40
CRVAL           equ     $06
                include "p3_body.s"
