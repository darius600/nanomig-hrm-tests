; p5b_ciab_tb_chip.s -- see p5b_body.s. Each sample reads a Chip-memory byte instead of the
; timer's LO register: the loop holds no CIA access.
MUT_CHIP         equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p5b_body.s"
