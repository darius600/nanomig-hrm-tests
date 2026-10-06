; P7.0, CIAB, START contrast (MUT_ST_NOSTART): CR<-$00 in place of $01 as the start write, so
; the timer never runs and u3=u2. See p_start_body.s.
MUT_ST_NOSTART  equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_start_body.s"
