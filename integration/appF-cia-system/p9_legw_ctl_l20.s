; p9_legw_ctl_l20.s -- see p9_legw_ctl_body.s.
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
SLATCH          equ     20
NSAMP           equ     200
                include "p9_legw_ctl_body.s"
