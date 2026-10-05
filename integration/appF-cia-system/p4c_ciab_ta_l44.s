; p4c_ciab_ta_l44.s -- see p4c_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
M               equ     $40
LATCHVAL        equ     44
CRVAL           equ     $01
XHI             equ     10
                include "p4c_body.s"
