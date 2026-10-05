; p9_legf_ciab_tb_l41_m.s -- P9-LEGF's contrast: the L=41 run made with a latch of 42.
; See p3_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
M               equ     $80
LATCHVAL        equ     42
CRVAL           equ     $07
                include "p3_body.s"
