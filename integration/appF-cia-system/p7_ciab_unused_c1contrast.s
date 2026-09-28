; P7.7, CIAB, "forced to 0 on a read" contrast: CR<-$02 (a used bit) in
; place of CR<-$80. See p_unused_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $02
SECONDVAL       equ     $81
                include "p_unused_body.s"
