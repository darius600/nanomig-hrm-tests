; p2_ciab_ta.s -- P2 instance (CIAB, timer A). See p2_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
N               equ     6
M               equ     $40
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
        include "p2_body.s"
