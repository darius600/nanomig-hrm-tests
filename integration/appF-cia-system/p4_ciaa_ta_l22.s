; p4_ciaa_ta_l22.s -- see p4_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
LATCHVAL        equ     22
CRVAL           equ     $03
                include "p4_body.s"
