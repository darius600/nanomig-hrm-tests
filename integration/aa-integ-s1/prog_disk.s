; prog_disk.s -- P13: the register list (DSKPTH/DSKPTL, INTREQ, DMACON, DSKLEN
; $4000, $C008, $C008), then poll INTREQR bit 1 (DSKBLK, AA p.25) storing each
; read to RESULTS+2 for at most 7000 reads (about 50 ms), write DSKLEN = $4000,
; then DONE.
        include "regs.i"
DSKLEN  equ     $024
        move.w  #6999,d3
.pd:    move.w  INTREQR(a0),d0
        move.w  d0,RESULTS+2
        btst    #1,d0
        bne.s   .got
        dbra    d3,.pd
.got:   move.w  #$4000,DSKLEN(a0)
        DONE
