; ch5_subs.i -- subroutines shared by every program in this package.
; Emits code: include this only after "org PROGRAM_ORG", and only after a
; guard branch over it (every program starts with "bra start"), since a
; fresh boot always fetches its first opcode at PROGRAM_ORG. Requires
; ch5_regs.i (equates) and nanomig.i already included.

; ---------------------------------------------------------------------
; setup_channel -- writes one audio channel's four registers.
;   entry: d0.w = register block offset (0/$10/$20/$30 for ch 0-3)
;          a1   = data-set address (32-bit, logical)
;          d1.w = AUDxLEN, d2.w = AUDxPER, d3.w = AUDxVOL
;   clobbers: a0 only (d0-d3, a1 are read, not written)
; The four registers sit at LCH+0/LCL+2 (one long write), LEN+4, PER+6,
; VOL+8 relative to the channel's own base (p.139: "writing a long word
; into AUDxLCH moves the audio data address into both locations").
setup_channel:
        lea     CUSTOM,a0
        adda.w  d0,a0
        adda.w  #AUD0LCH,a0
        move.l  a1,(a0)
        move.w  d1,4(a0)
        move.w  d2,6(a0)
        move.w  d3,8(a0)
        rts

; ---------------------------------------------------------------------
; run_session -- executes the program sequence for one session (steps
; 3-8 of SPECIFICATION.md's "Program sequence for one session"; steps
; 1-2, disabling everything and clearing ADKCON, are done once by the
; caller before the first call). Generic over up to four channels, from
; a descriptor block so that most sessions need no bespoke assembly.
;
;   entry: a2 = pointer to a session_desc:
;     +0  ch0 data ptr (long, 0 = channel not used this session)
;     +4  ch0 AUDxLEN (word)
;     +6  ch0 AUDxPER (word)
;     +8  ch0 AUDxVOL (word)
;     +10 ch1 data ptr / len / per / vol (same layout)
;     +20 ch2 data ptr / len / per / vol
;     +30 ch3 data ptr / len / per / vol
;     +40 ADKCON word for step 4 (e.g. $8001)
;     +42 DMACON word for step 6, enable (e.g. $8203)
;     +44 DMACON word for step 8, disable (e.g. $0003)
;     +46 duration in ticks for step 7 (long)
;   size: 50 bytes.
; An unused channel (data ptr 0) is skipped for LCH/LEN/PER, but its
; AUDxVOL is still written 0 (p.145's "Unused channels: AUDxVOL = 0 and
; AUDxEN clear" convention; AUDxEN clear comes from the DMACON words
; never setting that channel's bit).
;   clobbers: d0-d4, a0-a1 (caller-saved by this routine itself)
run_session:
        movem.l d0-d4/a0-a1,-(sp)
        lea     CUSTOM,a0
        moveq   #0,d4
.rs_ch:
        move.w  d4,d0
        mulu.w  #10,d0
        tst.l   (a2,d0.w)
        beq.s   .rs_ch_unused
        move.l  (a2,d0.w),a1
        move.w  4(a2,d0.w),d1
        move.w  6(a2,d0.w),d2
        move.w  8(a2,d0.w),d3
        move.w  d4,d0
        mulu.w  #$10,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        bra.s   .rs_ch_next
.rs_ch_unused:
        move.w  d4,d0
        mulu.w  #$10,d0
        adda.w  d0,a0
        move.w  #0,AUD0VOL(a0)
        lea     CUSTOM,a0
.rs_ch_next:
        addq.w  #1,d4
        cmp.w   #4,d4
        blt.s   .rs_ch

        move.w  40(a2),ADKCON(a0)      ; step 4
        move.w  ADKCONR(a0),ADKR_STORE ; step 5
        move.w  42(a2),DMACON(a0)      ; step 6
        move.w  #1,MARKER              ; M_on
        move.w  DMACONR(a0),DMACR_STORE

        move.l  46(a2),d0
        bsr     wait_ticks_via_lines   ; step 7

        move.w  44(a2),DMACON(a0)      ; step 8
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        movem.l (sp)+,d0-d4/a0-a1
        rts

; ---------------------------------------------------------------------
; wait_lines -- busy-waits for at least d0.w horizontal-line boundaries.
;   entry: d0.w = number of line boundaries to wait for (>=1)
;   clobbers: d0-d2, a0
; VHPOSR bits 7-0 are H8-H1, incrementing once per 280 ns colour-clock
; tick (p.285) and wrapping to a low value at the start of each new line
; (about 227 ticks/line on PAL, p.141's own derivation). A wrap is read
; back as a decrease. This is a stimulus-timing mechanism, not a graded
; quantity: it only has to wait *at least* the requested number of line
; boundaries, so a wrap detected by a plain decrease is enough.
wait_lines:
        lea     CUSTOM,a0
        move.w  VHPOSR(a0),d1
        and.w   #$00FF,d1
.wl_loop:
        move.w  VHPOSR(a0),d2
        and.w   #$00FF,d2
        cmp.w   d1,d2
        bge.s   .wl_nowrap
        subq.w  #1,d0
.wl_nowrap:
        move.w  d2,d1
        tst.w   d0
        bgt.s   .wl_loop
        rts

; ---------------------------------------------------------------------
; wait_ticks_via_lines -- convenience: waits for at least d0.l ticks by
; converting to a line count with a divisor safely under the true
; per-line tick count (so the wait always meets or exceeds the request)
; and calling wait_lines. Divisor 128 vs. the true ~227 ticks/line
; (p.141) leaves a wide, deliberate safety margin: precision is not
; needed here, only "at least".
;   entry: d0.l = number of ticks to wait for (>=1)
;   clobbers: d0-d2, a0
wait_ticks_via_lines:
        divu.w  #128,d0
        addq.w  #1,d0
        bra.s   wait_lines
