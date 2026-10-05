; p1_ciaa_ta_ep3.s -- EP3's own contrast: p1_ciaa_ta completes all of its
; work, then stores $600E (not DONE's $600D) as its final marker and loops.
        include "nanomig.i"
MUT_EP3         equ     1
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
