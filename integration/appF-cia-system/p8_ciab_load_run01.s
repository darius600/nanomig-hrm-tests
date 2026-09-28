; P8.4, CIAB, contrast (MUT_RUN01): CR<-$01 instead of $11 (no LOAD) in the
; running leg. See p_load_body.s.
MUT_RUN01       equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
