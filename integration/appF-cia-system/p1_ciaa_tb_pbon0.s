; p1_ciaa_tb_pbon0.s -- P1's own control (CIAA, timer B): every leg
; repeated with PBON = 0. See p1_ciaa_ta_pbon0.s and p1_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
N               equ     7
M               equ     $80
Q1              equ     %101010
Q2              equ     %010101
DDRB_INIT       equ     $00
MUT_PBON0       equ     1
        include "p1_body.s"
