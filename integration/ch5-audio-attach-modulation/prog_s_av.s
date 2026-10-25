; prog_s_av.s -- S-AV [P1, P3, P5, P8, P10]: ch0 (M_V, attach volume,
; ADKCON bit 0) modulates ch1 (SQ). See SPECIFICATION.md.
;
; Each define below changes the program of the session and nothing else:
; NOATTACH: the ADKCON $8001 write is omitted (PC1's contrast: the stored mask is $00).
; PER6000: AUD0PER=6000 instead of 4000 (P5's and P10's contrast: the page-predicted
;   spacing becomes wrong against the check's own D_mod from R-MOD).
; VOL1WRITE: ch1's own AUDxVOL is rewritten from 40 to 24 halfway through the
;   session (P1-B's contrast: 24 has a reference level pair in R-VOL-1 and is not a
;   word of M_V).
; SHIFT: AUD0PER=5000 at the start, rewritten to 4000 about 1000 ticks after M_on
;   (the contrast of the time-out grid lines of P5 and P10: the first time-out comes
;   5000 ticks after state 010 was entered, every later one 4000 ticks after the one
;   before, so the time-outs lie 1000 ticks off the grid of R-MOD).
; PER1WRITE: ch1's own AUDxPER is rewritten from 200 to 300 halfway through the
;   session (P1-C's contrast).
; SWAP: the second and third modulator words are exchanged (P1-A's contrast: every
;   value is still a word of M_V, the order is not).
; CH3: ch3 also plays, audibly, on the output channel 0 is observed on (P1-JOINT's
;   contrast for the source half).
; STOP: ch1's DMA is stopped after one eighth of the session (P1-WINDOW's contrast:
;   channel 1's output ends before channel 0 restarts).
; ZERO: every modulator word is 0 (P3-SILENT's contrast).
; CH1BYTES: ch1 plays the first four words of M_V as its own data (P3-WHOLE's
;   contrast: its words reach the output as two bytes each).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mv_data:
        ifd     SWAP
        dc.w    $0020,$0002,$0001,$0004,$0008,$0010,$003F,$0015
        else
        ifd     ZERO
        dc.w    $0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
        else
        dc.w    $0020,$0001,$0002,$0004,$0008,$0010,$003F,$0015
        endif
        endif
        org     CH1_DATA
sq_data:
        ifd     CH1BYTES
        dc.w    $0020,$0001,$0002,$0004
        else
        dc.w    $649C,$649C,$649C,$649C
        endif
        ifd     CH3
        org     CH3_DATA
wv_data3:       dc.w    $0030,$0010
        endif

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mv_data
        ifd     PER6000
        dc.w    8,6000,63
        else
        ifd     SHIFT
        dc.w    8,5000,63
        else
        dc.w    8,4000,63
        endif
        endif
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    0
        dc.w    0,0,0
        ifd     CH3
        dc.l    wv_data3
        dc.w    2,2000,63
        else
        dc.l    0
        dc.w    0,0,0
        endif
        ifd     NOATTACH
        dc.w    $8000
        else
        dc.w    $8001
        endif
        ifd     CH3
        dc.w    $820B
        dc.w    $000B
        else
        dc.w    $8203
        dc.w    $0003
        endif
        dc.l    200000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     PER1WRITE
        move.w  #1,d5
        move.w  #300,d6
        bsr     run_session_mut_ch_per
        else
        ifd     VOL1WRITE
        move.w  #1,d5
        move.w  #24,d6
        bsr     run_session_mut_ch_vol
        else
        ifd     SHIFT
        move.w  #4000,d6
        bsr     run_session_shift
        else
        ifd     STOP
        move.w  #$0002,d6
        bsr     run_session_stop_ch
        else
        bsr     run_session
        endif
        endif
        endif
        endif
        DONE

        ifd     SHIFT
        include "ch5_shift.i"
        endif

        ifd     STOP
; Like run_session, but after one eighth of the step-7 duration the DMACON bits in
; d6.w are cleared (those channels stop) and the rest of the duration is waited
; before step 8.
;   entry: a2 = pointer to a session_desc (as run_session); d6.w = DMACON clear word
run_session_stop_ch:
        movem.l d0-d4/d7/a0-a1,-(sp)
        lea     CUSTOM,a0
        moveq   #0,d4
.rssc_ch:
        move.w  d4,d0
        mulu.w  #10,d0
        tst.l   (a2,d0.w)
        beq.s   .rssc_unused
        move.l  (a2,d0.w),a1
        move.w  4(a2,d0.w),d1
        move.w  6(a2,d0.w),d2
        move.w  8(a2,d0.w),d3
        move.w  d4,d0
        mulu.w  #$10,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        bra.s   .rssc_next
.rssc_unused:
        move.w  d4,d0
        mulu.w  #$10,d0
        adda.w  d0,a0
        move.w  #0,AUD0VOL(a0)
        lea     CUSTOM,a0
.rssc_next:
        addq.w  #1,d4
        cmp.w   #4,d4
        blt.s   .rssc_ch

        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)
        move.w  #1,MARKER
        move.w  DMACONR(a0),DMACR_STORE

        move.l  46(a2),d7
        move.l  d7,d0
        lsr.l   #3,d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  d6,DMACON(a0)

        move.l  d7,d0
        lsr.l   #3,d0
        sub.l   d0,d7
        move.l  d7,d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)

        movem.l (sp)+,d0-d4/d7/a0-a1
        rts
        endif
