; prog_s_both.s -- S-BOTH [P4, P5, P8, P10]: ch0 (M_VP, both attach bits,
; ADKCON $11) modulates ch1's volume and period alternately.
; See SPECIFICATION.md.
;
; VOLONLY set: P4's own contrasting case (ADKCON $8001 instead of $8011:
; only bit 0 set; Table 5-4 "Modulating Volume Only" then predicts every
; word is a volume, so ch1's period stays 200).
; PER6000 set (prog_s_both_per6000.s): AUD0PER=6000 instead of 4000 (P5's
; contrasting case).
; PERWRITE set (with VOLONLY, prog_s_both_volonly_per.s): ch1's own AUDxPER is
; rewritten from 200 to 300 halfway through the session, so that its sample
; durations leave D_1(200) (the contrast of the S-BOTH-volonly line).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mvp_data:       dc.w    $0018,$012C,$0038,$02BC,$0008,$01F4,$0028,$0384
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mvp_data
        ifd     PER6000
        dc.w    8,6000,63
        else
        dc.w    8,4000,63
        endif
        dc.l    sq_data
        dc.w    4,200,63
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        ifd     VOLONLY
        dc.w    $8001
        else
        dc.w    $8011
        endif
        dc.w    $8203
        dc.w    $0003
        dc.l    110000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     PERWRITE
        move.w  #1,d5
        move.w  #300,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
