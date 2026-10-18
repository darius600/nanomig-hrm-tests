; prog_uhres.s -- P15: the register list, then after every VERTB (R5: clear
; INTREQ bit 5, poll INTREQR bit 5) rewrite BPLHPTH/BPLHPTL = U = $160000
; (AA p.6: the pointers "Must be reloaded by a processor before use"). After
; the fifth VERTB the program stores DONE.
        include "regs.i"
BPLHPTH equ     $1EC
BPLHPTL equ     $1EE
        moveq   #4,d3
.again: move.w  #$0020,INTREQ(a0)
.poll:  move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll
        move.w  #$0016,BPLHPTH(a0)
        move.w  #$0000,BPLHPTL(a0)
        dbra    d3,.again
        DONE
