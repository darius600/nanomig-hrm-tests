; P7.7, CIAB, "unaffected by a write" contrast: CR<-$21 (START and INMODE,
; used bits) in place of CR<-$81. See p_unused_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $21
                include "p_unused_body.s"
