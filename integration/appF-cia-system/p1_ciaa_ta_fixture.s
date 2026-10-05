; p1_ciaa_ta_fixture.s -- the fixture's own contrast: the program stores
; another constant ($BEE0) as its fixture word. Everything else is p1_ciaa_ta.
        include "nanomig.i"
FIXTURE_VALUE   equ     $BEE0
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
