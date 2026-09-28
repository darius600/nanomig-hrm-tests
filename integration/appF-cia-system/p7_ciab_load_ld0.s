; P7.4, CIAB, contrast (MUT_LD0): CR<-$00 instead of $10 ("writing a 0 has
; no effect"). See p_load_body.s.
MUT_LD0         equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
