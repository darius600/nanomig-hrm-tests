; p1_ciab_tb_pbon0_b0.s -- P1-CONTROL's own contrast (CIAB/timer B): the PBON=0
; run with leg B's data bit n written as 0 instead of 1. The page predicts
; b1 bit n = 0 there, not the PBON=0 control's 1. See p1_body.s.
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
MUT_B0          equ     1
        include "p1_body.s"
