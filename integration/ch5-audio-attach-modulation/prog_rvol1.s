; prog_rvol1.s -- R-VOL-1: channel 1 alone, unattached, plays SQ at PER
; 200. AUD1VOL is swept through VSET_1 in order, each value held for at
; least 4000 ticks, with a marker written right after each new value's
; register write (the marker before the first value is M_on itself,
; since the initial setup already writes VSET_1's first entry, 40).
; Serves PC4/PC5 and the reference level pairs L_1(v) used throughout.

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

        moveq   #0,d1
        move.w  #4,d1
        move.w  #200,d2
        move.w  #40,d3
        move.w  #$10,d0
        lea     sq_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8202,DMACON(a0)      ; DMAEN + AUD1EN
        move.w  #1,MARKER              ; M_on -- also value 40's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #4000,d0
        bsr     wait_ticks_via_lines

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

        move.w  #20,AUD1VOL(a0)
        move.w  #15,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #21,AUD1VOL(a0)
        move.w  #16,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #24,AUD1VOL(a0)
        move.w  #17,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #32,AUD1VOL(a0)
        move.w  #18,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #48,AUD1VOL(a0)
        move.w  #19,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #56,AUD1VOL(a0)
        move.w  #20,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #60,AUD1VOL(a0)
        move.w  #21,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #63,AUD1VOL(a0)
        move.w  #22,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0002,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
