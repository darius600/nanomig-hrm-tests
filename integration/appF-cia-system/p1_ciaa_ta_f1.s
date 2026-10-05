; p1_ciaa_ta_f1.s -- p1_ciaa_ta with shorter lines (HTOTAL, BEAMCON0) set after the first marker.
        include "nanomig.i"
MUT_F1         equ     1
        include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
N               equ     6
M               equ     $40
Q1              equ     %101010
Q2              equ     %010101
DDRB_INIT       equ     $00
        include "p1_body.s"
