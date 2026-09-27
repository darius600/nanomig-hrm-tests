; p1_ciab_tb_lp.s -- LP's own contrast (CIAB/timer B): bit 7 changes without
; running R-SELECT; the LED must keep its previous state in both legs
; (SPECIFICATION.md, O2's "Calibration precondition LP", Contrast). See
; p1_body.s.
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
MUT_LP          equ     1
        include "p1_body.s"
