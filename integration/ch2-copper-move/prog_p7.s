; prog_p7.s -- P7: with COPCON bit 1 (CDANG) set, a Copper MOVE writes a register
; in $40-$7E: a MOVE to BLTSIZE starts the blitter (SPECIFICATION.md P7). The
; lists at $3A000 and $3A100 are placed by the test.
;
; Symbols this source answers to:
;   P7_T       address the Copper starts at (default $3A000)
;   P7_EARLY   the program enables blitter DMA and writes BLTSIZE itself before the Copper starts
;   P7_NOCLEAR the CPU write of $0000 goes to $4F100 instead of $48000
;   P7_LF0     BLTCON0 holds LF = $00, so the blit writes $0000
;   P7_ATTR    a second CPU write to $48000 stands behind DONE, where it is never executed

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P7_T
P7_T            equ     $3A000
        endc

BLTCON0_R       equ     $040
BLTCON1_R       equ     $042
BLTAFWM_R       equ     $044
BLTALWM_R       equ     $046
BLTDPTH_R       equ     $054
BLTSIZE_R       equ     $058
BLTDMOD_R       equ     $066

        org     PROGRAM_ORG

start:
        BASELINE
        PARKPTRS
        move.w  #$0002,CUSTOM+COPCON
        ifd     P7_LF0
        move.w  #$0100,CUSTOM+BLTCON0_R
        else
        move.w  #$01FF,CUSTOM+BLTCON0_R
        endc
        move.w  #$0000,CUSTOM+BLTCON1_R
        move.w  #$FFFF,CUSTOM+BLTAFWM_R
        move.w  #$FFFF,CUSTOM+BLTALWM_R
        move.l  #$00048000,CUSTOM+BLTDPTH_R
        move.w  #$0000,CUSTOM+BLTDMOD_R
        ifnd    P7_NOCLEAR
        move.w  #$0000,$48000
        else
        move.w  #$0000,$4F100
        endc
        ifd     P7_EARLY
        move.w  #$8240,CUSTOM+DMACON
        move.w  #$0041,CUSTOM+BLTSIZE_R
        IDLEFIELDS 1
        endc
        move.l  #P7_T,CUSTOM+COP1LCH
        MARK    1
        move.w  #0,CUSTOM+COPJMP1
        MARK    2
        move.w  #$82C0,CUSTOM+DMACON
        MARK    3
        IDLEFIELDS 4
        move.w  $48000,RESULTS
        DONE
        ifd     P7_ATTR
        move.w  #$0000,$48000
        endc
