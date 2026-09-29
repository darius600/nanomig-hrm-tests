; P7.7's own mutation (NMH_MUTATE=P77-STARTOFF): CR<-$80 (START=0) in
; place of the bracketed CR<-$81 second write. See p_unused_body.s. Unlike
; P7.7-WCONTRAST (a used bit that does not distinguish itself on this
; build, the same INMODE gap P7.5/P8.65 mode 01 measure), this stimulus
; makes the w-clause fail through P7.7's own comparator: the timer never
; starts, so w stays at the force-loaded $8000 rather than tracking w01.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $80
                include "p_unused_body.s"
