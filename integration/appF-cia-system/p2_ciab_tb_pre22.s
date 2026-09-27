; p2_ciab_tb_pre22.s -- Pre2.2's own contrast (CIAB/timer B): leg S's own
; latch is $FFFF instead of $0020, so the first start's poll limit runs
; out with CR bit 0 still 1.
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
CIAB_PREWRITE   equ     1
DO_O2           equ     1
SLATCH          equ     $FFFF
        include "p2_body.s"
