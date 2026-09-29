; prog_rdacp4.s -- R-DAC-P4: channel 0 alone, unattached, plays M_P4.
; Serves P8's preconditions (the unattached reference for the DAC-bypass
; check) and PC3 (the specification's duration is a minimum; the length
; here is enough to show two complete cycles after the first restart).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
data0:  dc.w    $012C,$02BC,$01F4,$0384

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    data0
        dc.w    4,4000,63
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8000
        dc.w    $8201
        dc.w    $0001
        dc.l    60000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
