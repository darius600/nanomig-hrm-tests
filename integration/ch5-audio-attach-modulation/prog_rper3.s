; prog_rper3.s -- R-PER-3: channel 3 alone, unattached, plays SQ at VOL
; 40. AUD3PER swept through 200,300,500,700,900. See prog_rper1.s.

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
        move.w  #1,MARKER              ; M_on -- value 200's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #300,AUD3PER(a0)
        move.w  #10,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #500,AUD3PER(a0)
        move.w  #11,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #700,AUD3PER(a0)
        move.w  #12,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #900,AUD3PER(a0)
        move.w  #13,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0008,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
