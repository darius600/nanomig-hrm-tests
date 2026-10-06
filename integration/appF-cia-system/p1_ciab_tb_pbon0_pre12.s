; p1_ciab_tb_pbon0_pre12.s -- P1-CONTROL's Pre1.2 contrast (CIAB/timer B): the PBON=0
; run of p1_ciab_tb_pbon0.s with the Pre1.2 change of p1_ciab_tb_pre12.s:
; step 4's latch is $FFFF instead of $0010, so the poll limit runs out with CR bit 0 still 1 and the i1 flag at 0.
; See p1_body.s.
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
Q1              equ     %111101
Q2              equ     %111011
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
DO_O2           equ     1
MUT_PBON0       equ     1
MUT_PRE12       equ     1
        include "p1_body.s"
