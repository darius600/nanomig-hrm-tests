; p2_ciaa_tb.s -- P2 instance (CIAA, timer B). See p2_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
N               equ     7
M               equ     $80
DDRB_INIT       equ     $00
        include "p2_body.s"
