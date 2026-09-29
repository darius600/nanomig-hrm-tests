; p3_ciab_ta.s -- see p3_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
M               equ     $40
LATCHVAL        equ     60
CRVAL           equ     $07
                include "p3_body.s"
