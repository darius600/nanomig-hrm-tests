; p1_ciab_tb_pre14.s -- Pre1.4's own contrast (CIAB/timer B): the O1
; contrast leg's own data-bit-n write is left at 0 instead of 1, so the
; precondition's own "data 1 reads 1" half no longer holds. See p1_body.s.
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
MUT_PRE14       equ     1
        include "p1_body.s"
