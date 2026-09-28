; p1_ciab_tb_otherbits.s -- P1-OTHERBITS's own contrast (CIAB/timer B):
; leg B's own pattern bit 0 is written flipped, so b1's bits 0-5 no longer
; match Q1. The rest of the run (DDRB, CR, leg C's own Q2 write) is
; unchanged. See p1_body.s.
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
Q1              equ     %111100         ; %111101 with bit 0 flipped
Q2              equ     %111011
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
DO_O2           equ     1
        include "p1_body.s"
