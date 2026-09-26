; prog_fixture.s -- apparatus check for this package's own program
; conventions (not an HRM claim, no claim uid): a known constant is
; stored before anything reads one, and channel 0 played unattached,
; alone, shows more than one output value while its DMA is on.
; prog_fixture_mut.s (BROKEN set) stores a different constant, which the
; harness's fixture check must then fail on.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

start:
        ifd     BROKEN
        move.w  #$DEAD,FIXTURE_STORE
        else
        move.w  #$BEEF,FIXTURE_STORE
        endif

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)

        moveq   #0,d0
        lea     sq_data,a1
        move.w  #4,d1                  ; AUDxLEN, p.147: the word count
        move.w  #200,d2                ; AUDxPER
        move.w  #63,d3                  ; AUDxVOL
        bsr     setup_channel
        lea     CUSTOM,a0

        move.w  #$8201,DMACON(a0)      ; DMAEN + AUD0EN
        move.w  #1,MARKER
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  DMACONR(a0),DMACR_STORE

        move.l  #2000,d0
        bsr     wait_ticks_via_lines

        move.w  #$0001,DMACON(a0)      ; clear AUD0EN
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)

        DONE
