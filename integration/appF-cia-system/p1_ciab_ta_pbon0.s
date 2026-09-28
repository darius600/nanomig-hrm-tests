; p1_ciab_ta_pbon0.s -- P1's own control (CIAB, timer A): every leg
; repeated with PBON = 0. See p1_ciaa_ta_pbon0.s and p1_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
N               equ     6
M               equ     $40
Q1              equ     %111101
Q2              equ     %111011
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
MUT_PBON0       equ     1
        include "p1_body.s"
