; ch5_shift.i -- the SHIFT variants of S-AV, S-AP and S-BOTH and the PULSE variant of
; S-AP. Included only under "ifd SHIFT" or "ifd PULSE", after ch5_subs.i.

; ---------------------------------------------------------------------
; run_session_pulse -- like run_session, but about 1000 ticks after M_on channel 0's
; AUDxPER is rewritten to the value in d5.w, and about 4500 ticks (twenty lines) after
; that to the value in d6.w; the rest of the step-7 duration is waited before step 8.
; The first write lies after state 010 was entered and before its first time-out, the
; second after the counter was reloaded with the first value and before it times out
; again, so the half-loop that starts at the first time-out lasts the first value
; instead of the session's AUD0PER.
;   entry: a2 = pointer to a session_desc (as run_session)
;          d5.w = the first new AUD0PER, d6.w = the second
;   clobbers: d0-d6, a0-a1
run_session_pulse:
        movem.l d0-d6/a0-a1,-(sp)
        lea     CUSTOM,a0
        moveq   #0,d4
.rspu_ch:
        move.w  d4,d0
        mulu.w  #10,d0
        tst.l   (a2,d0.w)
        beq.s   .rspu_unused
        move.l  (a2,d0.w),a1
        move.w  4(a2,d0.w),d1
        move.w  6(a2,d0.w),d2
        move.w  8(a2,d0.w),d3
        move.w  d4,d0
        mulu.w  #$10,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        bra.s   .rspu_next
.rspu_unused:
        move.w  d4,d0
        mulu.w  #$10,d0
        adda.w  d0,a0
        move.w  #0,AUD0VOL(a0)
        lea     CUSTOM,a0
.rspu_next:
        addq.w  #1,d4
        cmp.w   #4,d4
        blt.s   .rspu_ch

        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)
        move.w  #1,MARKER
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #1000,d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  d5,AUD0PER(a0)

        move.l  #2500,d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  d6,AUD0PER(a0)

        move.l  46(a2),d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)

        movem.l (sp)+,d0-d6/a0-a1
        rts

; ---------------------------------------------------------------------
; run_session_shift -- like run_session, but about 1000 ticks after M_on (after
; state 010 was entered and before its first time-out, for the session's AUD0PER
; of 5000) channel 0's AUDxPER is rewritten to the value in d6.w, and the rest of
; the step-7 duration is waited before step 8.
;   entry: a2 = pointer to a session_desc (as run_session)
;          d6.w = the new AUD0PER
;   clobbers: d0-d6, a0-a1
run_session_shift:
        movem.l d0-d6/a0-a1,-(sp)
        lea     CUSTOM,a0
        moveq   #0,d4
.rssh_ch:
        move.w  d4,d0
        mulu.w  #10,d0
        tst.l   (a2,d0.w)
        beq.s   .rssh_unused
        move.l  (a2,d0.w),a1
        move.w  4(a2,d0.w),d1
        move.w  6(a2,d0.w),d2
        move.w  8(a2,d0.w),d3
        move.w  d4,d0
        mulu.w  #$10,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        bra.s   .rssh_next
.rssh_unused:
        move.w  d4,d0
        mulu.w  #$10,d0
        adda.w  d0,a0
        move.w  #0,AUD0VOL(a0)
        lea     CUSTOM,a0
.rssh_next:
        addq.w  #1,d4
        cmp.w   #4,d4
        blt.s   .rssh_ch

        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)
        move.w  #1,MARKER
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #1000,d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  d6,AUD0PER(a0)

        move.l  46(a2),d0
        bsr     wait_ticks_via_lines

        lea     CUSTOM,a0
        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)

        movem.l (sp)+,d0-d6/a0-a1
        rts
