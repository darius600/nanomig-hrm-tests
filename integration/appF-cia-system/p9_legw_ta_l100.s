; p9_legw_ta_l100.s -- P9 leg W, CIAB/timer A, L=100. See p9_legw_body.s.
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
SLATCH          equ     100
NSAMP           equ     300
                include "p9_legw_body.s"
