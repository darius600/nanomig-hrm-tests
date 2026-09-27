; p2_ciab_tb_contrast.s -- P2's own contrasting case (CIAB/timer B): start
; 4 is written as CR<-$0E (START=0) instead of $0F. The page predicts no
; start, so s_4 reads 0 throughout with no rise, and "at least one 1"
; fails for k=4. Every precondition of start 4 still holds (start 3's
; one-shot ended, and z_3=0), so the earlier k's still grade normally.
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
S4VAL           equ     $0E
        include "p2_body.s"
