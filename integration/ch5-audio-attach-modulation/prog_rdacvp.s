; prog_rdacvp.s -- R-DAC-VP: channel 0 alone, unattached, plays M_VP.
; Serves P8's preconditions and PC3 (the specification's duration is a
; minimum; the length here is enough to show two complete cycles after
; the first restart).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
data0:  dc.w    $0018,$012C,$0038,$02BC,$0008,$01F4,$0028,$0384

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    data0
        dc.w    8,4000,63
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8000
        dc.w    $8201
        dc.w    $0001
        dc.l    125000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
