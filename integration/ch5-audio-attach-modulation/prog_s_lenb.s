; prog_s_lenb.s -- S-LEN-B [P9]: ch0 (M_LB, 2 words, attach volume,
; ADKCON bit 0) modulates ch1 (SQ, 7 words) -- lengths coprime, modulator
; shorter than modulated. See SPECIFICATION.md, P9.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mlb_data:       dc.w    $0008,$0030
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mlb_data
        dc.w    2,4000,63
        dc.l    sq_data
        dc.w    7,200,40
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8001
        dc.w    $8203
        dc.w    $0003
        dc.l    60000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
