; p1_ciab_tb_pbon0_pre11.s -- P1-CONTROL's Pre1.1 contrast (CIAB/timer B): the PBON=0
; run of p1_ciab_tb_pbon0.s with the Pre1.1 change of p1_ciab_tb_pre11.s: DDRB bit n is set to output with the data bit 0 before the pull-up
; reference is read, so r0 bit n reads 0 instead of the pull-up's 1.
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
MUT_PRE11       equ     1
        include "p1_body.s"
