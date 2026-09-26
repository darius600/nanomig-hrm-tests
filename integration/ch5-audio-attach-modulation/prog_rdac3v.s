; prog_rdac3v.s -- R-DAC3-V: channel 3 alone, unattached, plays W_V.
; Serves P8's preconditions (channel 3's own unattached DAC reference).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH3_DATA
data3:  dc.w    $0030,$0010

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    data3
        dc.w    2,2000,63
        dc.w    $8000
        dc.w    $8208
        dc.w    $0008
        dc.l    30000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
