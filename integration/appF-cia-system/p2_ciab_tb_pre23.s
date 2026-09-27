; p2_ciab_tb_pre23.s -- Pre2.3's own contrast (CIAB/timer B): start 1 is
; written as CR<-$0D (PBON=0) instead of $0F. That one-shot still ends
; (Pre2.2 still holds), but with DDRB bit n=0 (PBON off) the line reads
; its pull-up: z_1=1, so start 2's own level-before is not 0 and start 2
; is not graded.
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
S1VAL           equ     $0D
        include "p2_body.s"
