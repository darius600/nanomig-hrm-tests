; p1_ciaa_ta_pbon0.s -- P1's own control (CIAA, timer A): every leg
; repeated with PBON = 0. The page predicts a2,a1,b1 bit n=1, c1 bit n=0,
; d1 bit n=0, d2 bit n=1 (SPECIFICATION.md P1, "P1's control"). See
; p1_body.s.
        include "nanomig.i"
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
MUT_PBON0       equ     1
        include "p1_body.s"
