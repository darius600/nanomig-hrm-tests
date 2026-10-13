; prog_s_ap.s -- S-AP [P5, P8, P10]: ch0 (M_P4, attach period, ADKCON bit 4)
; modulates ch1's period (SQ, unaffected in volume). See SPECIFICATION.md.
; PER6000: AUD0PER=6000 instead of 4000 (P5's contrast).
; SWAP: the second and third modulator words are exchanged (P8-JOINT-AP's contrast
;   for the destination half: every value is still a word of M_P4, the order is not).
; SHORT: the session lasts 25500 ticks (P5-POP-AP's contrast).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp4_data:
        ifd     SWAP
        dc.w    $012C,$01F4,$02BC,$0384
        else
        dc.w    $012C,$02BC,$01F4,$0384
        endif
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mp4_data
        ifd     PER6000
        dc.w    4,6000,63
        else
        dc.w    4,4000,63
        endif
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8010
        dc.w    $8203
        dc.w    $0003
        ifd     SHORT
        dc.l    25500
        else
        dc.l    110000
        endif

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
