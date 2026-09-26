; prog_rvol3.s -- R-VOL-3: channel 3 alone, unattached, plays SQ at PER
; 200. AUD3VOL swept through VSET_3 = {40,8,20,48,60}.
; See prog_rvol1.s for the pattern.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH3_DATA
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
        move.w  #40,d3
        move.w  #$30,d0
        lea     sq_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8208,DMACON(a0)      ; DMAEN + AUD3EN
        move.w  #1,MARKER              ; M_on -- value 40's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #8,AUD3VOL(a0)
        move.w  #10,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #20,AUD3VOL(a0)
        move.w  #11,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #48,AUD3VOL(a0)
        move.w  #12,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #60,AUD3VOL(a0)
        move.w  #13,MARKER
        move.l  #4000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0008,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
