; p1_ciaa_tb_ep1.s -- p1_ciaa_tb with a write to CIAA CRA before SETUP's first read of it.
        include "nanomig.i"
MUT_EP1         equ     1
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
        include "p1_body.s"
