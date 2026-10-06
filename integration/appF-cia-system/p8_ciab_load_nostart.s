; P8.4, CIAB, Pre8.4's contrast (MUT_NOSTART): CR<-$00 instead of $01 in the stopped leg, so
; the counter does not move. See p_load_body.s.
MUT_NOSTART     equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
