; p2_ciab_tb_legh0.s -- P2-LEGH's own START=0 mutation (CIAB/timer B):
; h1's own start write is CR<-$06 (START=0) instead of $07, so the timer
; is never started for h1's own read. See p2_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
N               equ     7
M               equ     $80
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
DO_O2           equ     1
H1VAL           equ     $06
        include "p2_body.s"
