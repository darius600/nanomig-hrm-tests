; prog_rper2.s -- R-PER-2: channel 2 alone, unattached, plays SQ at VOL
; 40. AUD2PER swept through 200,300,500,700,900. See prog_rper1.s.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH2_DATA
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
        move.w  #$20,d0
        lea     sq_data,a1
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8000,ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  #$8204,DMACON(a0)      ; DMAEN + AUD2EN
        move.w  #1,MARKER              ; M_on -- value 200's own start
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #300,AUD2PER(a0)
        move.w  #10,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #500,AUD2PER(a0)
        move.w  #11,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #700,AUD2PER(a0)
        move.w  #12,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #900,AUD2PER(a0)
        move.w  #13,MARKER
        move.l  #20000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0004,DMACON(a0)
        move.w  #2,MARKER              ; M_off
        move.w  #$00FF,ADKCON(a0)

        DONE
