; p2_ciab_tb_pre21.s -- Pre2.1's own contrast (CIAB/timer B): an extra
; start (R-LATCH($FFFF), CR<-$07) is inserted before leg R's own write.
; The page then predicts r1=1 (started, no underflow) and LED-R off.
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
MUT_PRE21       equ     1
        include "p2_body.s"
