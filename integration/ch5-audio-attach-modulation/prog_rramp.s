; prog_rramp.s -- R-RAMP: channel 1 alone, unattached, plays RAMP at VOL
; 48. AUD1PER is written 700 (settle, not graded -- P2's window explicitly
; starts only once the applied period differs from 700) and then to each
; M_P value in M_P order. Each hold is the page-derived formula
; (previous value + 21*value if value<=4000, else previous value +
; 3*value) ticks, computed once at build time from the M_P table and
; written here as constants -- SPECIFICATION.md's "Reference
; duration"/"Sessions" convention, not a value tuned on the RTL.
; Serves PC4 and the reference durations D_1(P) for every M_P value,
; used by P2.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH1_DATA
ramp_data:      dc.w    $0818,$2838,$4858,$6878,$F8E8,$D8C8,$B8A8,$9888

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)

        move.w  #8,d1
        move.w  #700,d2
        move.w  #48,d3
        move.w  #$10,d0
        lea     ramp_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8202,DMACON(a0)      ; DMAEN + AUD1EN
        move.w  #1,MARKER              ; M_on -- the 700 settle phase
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #5000,d0               ; settle: not graded (P2 discards it)
        bsr     wait_ticks_via_lines

        move.w  #400,AUD1PER(a0)
        move.w  #10,MARKER
        move.l  #9100,d0
        bsr     wait_ticks_via_lines

        move.w  #401,AUD1PER(a0)
        move.w  #11,MARKER
        move.l  #8821,d0
        bsr     wait_ticks_via_lines

        move.w  #402,AUD1PER(a0)
        move.w  #12,MARKER
        move.l  #8843,d0
        bsr     wait_ticks_via_lines

        move.w  #404,AUD1PER(a0)
        move.w  #13,MARKER
        move.l  #8886,d0
        bsr     wait_ticks_via_lines

        move.w  #408,AUD1PER(a0)
        move.w  #14,MARKER
        move.l  #8972,d0
        bsr     wait_ticks_via_lines

        move.w  #384,AUD1PER(a0)
        move.w  #15,MARKER
        move.l  #8472,d0
        bsr     wait_ticks_via_lines

        move.w  #432,AUD1PER(a0)
        move.w  #16,MARKER
        move.l  #9456,d0
        bsr     wait_ticks_via_lines

        move.w  #464,AUD1PER(a0)
        move.w  #17,MARKER
        move.l  #10176,d0
        bsr     wait_ticks_via_lines

        move.w  #272,AUD1PER(a0)
        move.w  #18,MARKER
        move.l  #6176,d0
        bsr     wait_ticks_via_lines

        move.w  #16784,AUD1PER(a0)
        move.w  #19,MARKER
        move.l  #50624,d0
        bsr     wait_ticks_via_lines

        move.w  #144,AUD1PER(a0)
        move.w  #20,MARKER
        move.l  #19808,d0
        bsr     wait_ticks_via_lines

        move.w  #33168,AUD1PER(a0)
        move.w  #21,MARKER
        move.l  #99648,d0
        bsr     wait_ticks_via_lines

        move.w  #912,AUD1PER(a0)
        move.w  #22,MARKER
        move.l  #52320,d0
        bsr     wait_ticks_via_lines

        move.w  #8592,AUD1PER(a0)
        move.w  #23,MARKER
        move.l  #26688,d0
        bsr     wait_ticks_via_lines

        move.w  #1424,AUD1PER(a0)
        move.w  #24,MARKER
        move.l  #38496,d0
        bsr     wait_ticks_via_lines

        move.w  #4496,AUD1PER(a0)
        move.w  #25,MARKER
        move.l  #14912,d0
        bsr     wait_ticks_via_lines

        move.w  #2448,AUD1PER(a0)
        move.w  #26,MARKER
        move.l  #55904,d0
        bsr     wait_ticks_via_lines

        move.w  #$0002,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
