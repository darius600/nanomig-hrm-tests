; P9 leg W, CIAB/timer B, L=100. See p9_legw_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
N               equ     7
M               equ     $80
DDRB_INIT       equ     $08
SLATCH          equ     100
NSAMP           equ     300
                include "p9_legw_body.s"
