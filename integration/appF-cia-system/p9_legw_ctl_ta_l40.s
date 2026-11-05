; p9_legw_ctl_ta_l40.s -- see p9_legw_ctl_body.s. CIAB/timer A, L=40.
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
ICRBIT          equ     0
DDRB_INIT       equ     $08
SLATCH          equ     40
NSAMP           equ     200
                include "p9_legw_ctl_body.s"
