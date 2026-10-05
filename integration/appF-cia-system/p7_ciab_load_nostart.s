; P7.4, CIAB, Pre7.4's contrast (MUT_NOSTART): CR<-$00 instead of $01 in the stopped leg, so
; the counter does not move. See p_load_body.s.
MUT_NOSTART     equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
