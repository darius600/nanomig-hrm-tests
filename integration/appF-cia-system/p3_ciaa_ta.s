; p3_ciaa_ta.s -- see p3_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
M               equ     $40
LATCHVAL        equ     60
CRVAL           equ     $07
                include "p3_body.s"
