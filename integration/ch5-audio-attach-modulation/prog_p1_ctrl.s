; prog_p1_ctrl.s -- P1's own honest control: the CPU writes AUD1VOL
; directly, unattached, in M_V's own order (32, 1, 2, 4, 8, 16, 63, 21),
; each value held for 4000 ticks as R-VOL-1's own sweep does, with a
; marker right after each write (as every reference sweep does), so the
; settling runs can be discarded per interval the same way R-VOL-1's own
; level_table already does. The full M_V cycle runs three times (24
; markers, plus the initial value written by setup_channel: 25 values,
; comfortably >=17). Serves P1's own MUTATION: the same comparator
; (cyclic-consecutive-in-M_V, level pairs matching L_1(v)) graded against a
; proven-live stimulus instead of the (already-red) attach mechanism.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)

        move.w  #4,d1
        move.w  #200,d2
        move.w  #32,d3                 ; M_V's first value
        move.w  #$10,d0
        lea     sq_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)      ; no attach bits
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8202,DMACON(a0)      ; DMAEN + AUD1EN
        move.w  #1,MARKER              ; M_on -- also value 32's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        ; cycle 1 (markers 10-17)
        move.w  #1,AUD1VOL(a0)
        move.w  #10,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #2,AUD1VOL(a0)
        move.w  #11,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #4,AUD1VOL(a0)
        move.w  #12,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #8,AUD1VOL(a0)
        move.w  #13,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #16,AUD1VOL(a0)
        move.w  #14,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #63,AUD1VOL(a0)
        move.w  #15,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #21,AUD1VOL(a0)
        move.w  #16,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #32,AUD1VOL(a0)
        move.w  #17,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        ; cycle 2 (markers 18-25)
        move.w  #1,AUD1VOL(a0)
        move.w  #18,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #2,AUD1VOL(a0)
        move.w  #19,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #4,AUD1VOL(a0)
        move.w  #20,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #8,AUD1VOL(a0)
        move.w  #21,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #16,AUD1VOL(a0)
        move.w  #22,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #63,AUD1VOL(a0)
        move.w  #23,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #21,AUD1VOL(a0)
        move.w  #24,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #32,AUD1VOL(a0)
        move.w  #25,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        ; cycle 3 (markers 26-33)
        move.w  #1,AUD1VOL(a0)
        move.w  #26,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #2,AUD1VOL(a0)
        move.w  #27,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #4,AUD1VOL(a0)
        move.w  #28,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #8,AUD1VOL(a0)
        move.w  #29,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #16,AUD1VOL(a0)
        move.w  #30,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #63,AUD1VOL(a0)
        move.w  #31,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #21,AUD1VOL(a0)
        move.w  #32,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines
        move.w  #32,AUD1VOL(a0)
        move.w  #33,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0002,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
