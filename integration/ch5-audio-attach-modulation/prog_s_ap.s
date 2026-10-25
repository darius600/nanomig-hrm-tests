; prog_s_ap.s -- S-AP [P5, P8, P10]: ch0 (M_P4, attach period, ADKCON bit 4)
; modulates ch1's period (SQ, unaffected in volume). See SPECIFICATION.md.
; PER6000: AUD0PER=6000 instead of 4000 (the contrast of the spacing and rate lines of
;   P5 and P10).
; SHIFT: AUD0PER=5000 at the start, rewritten to 4000 about 1000 ticks after M_on
;   (the contrast of the time-out grid lines of P5 and P10: every time-out lies 1000
;   ticks off the grid of R-MOD).
; PULSE: AUD0PER=12000 about 1000 ticks after M_on and 4000 again about 4500 ticks
;   later (the contrast of the rate clause of P10-AP: the half-loop that starts at the
;   first time-out lasts 12000 ticks instead of 4000, so every later request, and every
;   change it carries, comes 8000 ticks = 2*D_mod later than the loop of R-MOD, which is
;   the same phase of the 2*D_mod loop).
; SWAP: the second and third modulator words are exchanged (P8-JOINT-AP's contrast
;   for the destination half: every value is still a word of M_P4, the order is not).

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
        ifd     SHIFT
        dc.w    4,5000,63
        else
        dc.w    4,4000,63
        endif
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
        dc.l    110000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     SHIFT
        move.w  #4000,d6
        bsr     run_session_shift
        else
        ifd     PULSE
        move.w  #12000,d5
        move.w  #4000,d6
        bsr     run_session_pulse
        else
        bsr     run_session
        endif
        endif
        DONE

        ifd     SHIFT
        include "ch5_shift.i"
        endif
        ifd     PULSE
        include "ch5_shift.i"
        endif
