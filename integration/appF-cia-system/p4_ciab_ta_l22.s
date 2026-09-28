; p4_ciab_ta_l22.s -- see p4_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     22
CRVAL           equ     $03
                include "p4_body.s"
