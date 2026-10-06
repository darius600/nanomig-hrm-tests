; prog_rper1.s -- R-PER-1: channel 1 alone, unattached, plays SQ at VOL
; 40. AUD1PER swept through 200,300,500,700,900 (200 already written by
; the initial setup, so M_on marks its own start), each held for at least
; 20000 ticks. Serves PC4 and the reference durations D_1(P).
; MUT500 set (prog_rper1_mut.s): the step labelled 700 writes 500 (PC4-DUR's own
; contrast).

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
        move.w  #40,d3
        move.w  #$10,d0
        lea     sq_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8202,DMACON(a0)      ; DMAEN + AUD1EN
        move.w  #1,MARKER              ; M_on -- value 200's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #300,AUD1PER(a0)
        move.w  #10,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #500,AUD1PER(a0)
        move.w  #11,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        ifd     MUT500
        move.w  #500,AUD1PER(a0)       ; the step labelled 700 writes 500
        else
        move.w  #700,AUD1PER(a0)
        endif
        move.w  #12,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #900,AUD1PER(a0)
        move.w  #13,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0002,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
