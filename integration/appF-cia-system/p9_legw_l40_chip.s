; p9_legw_l40_chip.s -- see p9_legw_body.s. Each sample reads a Chip-memory byte instead of PRB.
MUT_CHIP        equ     1
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
                include "p9_legw_body.s"
