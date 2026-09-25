; prog.s -- apparatus check for vasm-built test programs (not an HRM claim).
; Stores known values where the harness reads them, reads two custom
; registers for the record, and ends with DONE. prog_control.s assembles
; this same file with BROKEN set, which changes the first stored value.

        include "nanomig.i"
        org     PROGRAM_ORG

start:
        lea     RESULTS,a1
        ifd     BROKEN
        move.w  #$1235,(a1)+            ; the control: one value off
        else
        move.w  #$1234,(a1)+
        endif
        move.l  #$CAFEF00D,(a1)+
        moveq   #4,d0
.count: move.w  d0,(a1)+                ; 4, 3, 2, 1, 0
        dbra    d0,.count
        lea     CUSTOM,a0
        move.w  DMACONR(a0),(a1)+       ; recorded, not graded
        move.w  VPOSR(a0),(a1)+         ; recorded, not graded
        DONE
