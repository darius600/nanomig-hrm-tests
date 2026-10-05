; p9_legw_ctl_l40_lvl1.s -- see p9_legw_ctl_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
N               equ     7
M               equ     $80
DDRB_INIT       equ     $08
SLATCH          equ     40
NSAMP           equ     200
LVL1            equ     1
                include "p9_legw_ctl_body.s"
