; prog_p8.s -- P8: whenever an audio channel's length count finishes, its pointer
; restarts from the channel's location registers (SPECIFICATION.md P8). The
; data blocks at $44000 and $44100 are placed by the test.
;
; Symbols this source answers to:
;   P8_RUNC    run C: about 30 lines after marker 2 the CPU writes AUD0LC = $44100, then marker 4
;   P8_NOLC    run C without the AUD0LC write (marker 4 stays)
;   P8_LEN     AUD0LEN (default 4)
;   P8_NOAUD   DMACON <- $8200 (no AUD0EN)
;   P8_EARLY   DMACON <- $8201 after the AUD0PER write and before marker 1
;   P8_ATTR    a CPU read of the audio data block stands behind DONE, where it is never executed

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P8_LEN
P8_LEN          equ     4
        endc
        ifd     P8_NOAUD
P8_EN           equ     $8200
        else
P8_EN           equ     $8201
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        move.w  #$00FF,CUSTOM+ADKCON
        PARKPTRS
        move.l  #$00044000,CUSTOM+AUD0LCH
        move.w  #P8_LEN,CUSTOM+AUD0LEN
        move.w  #32,CUSTOM+AUD0VOL
        move.w  #256,CUSTOM+AUD0PER
        ifd     P8_EARLY
        move.w  #P8_EN,CUSTOM+DMACON
        endc
        MARK    1
        WAITLINE $40
        ifnd    P8_EARLY
        move.w  #P8_EN,CUSTOM+DMACON
        endc
        MARK    2
        ifd     P8_SHORT
        move.w  #$00C8,CUSTOM+HTOTAL
        move.w  #$0080,CUSTOM+BEAMCON0
        endc
        ifd     P8_RUNC
.wl     move.b  CUSTOM+VHPOSR,d0
        cmp.b   #$5F,d0
        bne.s   .wl
        ifnd    P8_NOLC
        move.l  #$00044100,CUSTOM+AUD0LCH
        endc
        MARK    4
        endc
        IDLEFIELDS 2
        MARK    3
        DONE
        ifd     P8_ATTR
        move.w  $44000,d1
        endc
