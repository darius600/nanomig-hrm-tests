; p1_ciaa_tb_ep3.s -- p1_ciaa_tb that completes its work and stores $600E, not DONE_VALUE, as its final marker.
        include "nanomig.i"
MUT_EP3         equ     1
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
