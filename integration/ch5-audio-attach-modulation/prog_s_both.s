; prog_s_both.s -- S-BOTH [P4, P5, P8, P10]: ch0 (M_VP, both attach bits,
; ADKCON $11) modulates ch1's volume and period alternately.
; See SPECIFICATION.md.
;
; Each define below changes the program of the session and nothing else:
; VOLONLY: ADKCON $8001 instead of $8011 (only bit 0 set; Table 5-4 "Modulating
;   Volume Only" then predicts every word is a volume, so ch1's period stays 200).
; PER6000 (prog_s_both_per6000.s): AUD0PER=6000 instead of 4000, and the session lasts
;   250000 ticks (the contrast of the spacing line of P5: the page-predicted spacing
;   becomes wrong against the check's own D_mod from R-MOD; the session is long enough
;   for two complete cycles of the modulator after its first restart).
; SHIFT (prog_s_both_shift.s): AUD0PER=5000 at the start, rewritten to 4000 about 1000
;   ticks after M_on (the contrast of the time-out grid lines of P5 and P10: every
;   time-out lies 1000 ticks off the grid of R-MOD).
; PER4050 (prog_s_both_per4050.s): AUD0PER=4050 instead of 4000 (the contrast of the grid
;   clause of P10-BOTH: consecutive requests are 4050 ticks apart, less than one line
;   from D_mod, and each lies 50 ticks further from the grid of R-MOD than the one before).
; PERWRITE (with VOLONLY, prog_s_both_volonly_per.s): ch1's own AUDxPER is
;   rewritten from 200 to 300 halfway through the session, so that its sample
;   durations leave D_1(200) (the contrast of the S-BOTH-volonly line).
; SWAP: the volume words 56 and 8 are exchanged (P4-A's contrast: every value is
;   still a word of M_VP, the order is not).
; V32: the last volume word is 32 instead of 40, a volume outside the four of M_VP
;   (P4-B's contrast).
; VOLWORDS (prog_s_both_volwords.s): every period-position word is 200, the period channel 1
;   already holds (the contrast of the population and alternation clauses of P5-BOTH: only the
;   volume words change anything).
; PERCONST: every period word is 300 (P4-C's contrast: the period changes once).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mvp_data:
        ifd     SWAP
        dc.w    $0018,$012C,$0008,$02BC,$0038,$01F4,$0028,$0384
        else
        ifd     V32
        dc.w    $0018,$012C,$0038,$02BC,$0008,$01F4,$0020,$0384
        else
        ifd     PERCONST
        dc.w    $0018,$012C,$0038,$012C,$0008,$012C,$0028,$012C
        else
        ifd     VOLWORDS
        dc.w    $0018,$00C8,$0038,$00C8,$0008,$00C8,$0028,$00C8
        else
        dc.w    $0018,$012C,$0038,$02BC,$0008,$01F4,$0028,$0384
        endif
        endif
        endif
        endif
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
        ifd     SHIFT
        dc.w    8,5000,63
        else
        ifd     PER4050
        dc.w    8,4050,63
        else
        dc.w    8,4000,63
        endif
        endif
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
        ifd     PER6000
        dc.l    250000
        else
        dc.l    110000
        endif

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
        ifd     SHIFT
        move.w  #4000,d6
        bsr     run_session_shift
        else
        bsr     run_session
        endif
        endif
        DONE

        ifd     SHIFT
        include "ch5_shift.i"
        endif
