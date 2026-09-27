; p2_ciaa_ta.s -- P2 instance (CIAA, timer A). See p2_body.s.
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
DDRB_INIT       equ     $00
        include "p2_body.s"
