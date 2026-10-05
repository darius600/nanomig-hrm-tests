; P7.7's START-bit mutation (NMH_MUTATE=P77-STARTOFF): CR<-$80 (START=0) in
; place of the bracketed CR<-$81 second write. See p_unused_body.s. The timer
; never starts, so w stays at the latch value $8000 instead of tracking w01, and
; c2 reads $00 instead of $01.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $80
                include "p_unused_body.s"
