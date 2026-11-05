; p9_legf_ciab_ta_l41_m.s -- P9-LEGF's contrast on CIAB/timer A: the L=41 run made with a latch of 42. Reuses p3_body.s (P3's own
; mechanism: toggle mode, R-SAMPLE(600;PRB,ICR)).
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
M               equ     $40
LATCHVAL        equ     42
CRVAL           equ     $07
                include "p3_body.s"
