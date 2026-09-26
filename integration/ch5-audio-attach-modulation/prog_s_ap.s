; prog_s_ap.s -- S-AP [P5, P10]: ch0 (M_P4, attach period, ADKCON bit 4)
; modulates ch1's period (SQ, unaffected in volume). See SPECIFICATION.md.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp4_data:       dc.w    $012C,$02BC,$01F4,$0384
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mp4_data
        dc.w    4,4000,63
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8010
        dc.w    $8203
        dc.w    $0003
        dc.l    110000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
