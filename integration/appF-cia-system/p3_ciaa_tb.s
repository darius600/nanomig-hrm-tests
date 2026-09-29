; p3_ciaa_tb.s -- see p3_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
M               equ     $80
LATCHVAL        equ     60
CRVAL           equ     $07
                include "p3_body.s"
