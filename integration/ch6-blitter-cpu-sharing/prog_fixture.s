; prog_fixture.s -- fixture check (no claim id): the package's own control.
; Stores a known constant to RESULTS, then runs one minimal channel A-to-D
; word blit copying a known source word to a known destination word, then
; DONE. Every other program in this package that trusts RESULTS or a
; blit's destination content depends on this fixture passing first.

        include "nanomig.i"

FIX_SRC equ     $0C2000
FIX_DST equ     $0C2100

        org     PROGRAM_ORG

start:
        move.w  #$BEEF,RESULTS

        move.w  #$DEAD,FIX_SRC

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN
        move.w  #$09F0,BLTCON0(a0)     ; USEA,USED, LF=$F0 (D=A straight copy)
        move.w  #$0000,BLTCON1(a0)     ; ascending, normal (not line) mode
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #FIX_SRC,BLTAPTH(a0)
        move.l  #FIX_DST,BLTDPTH(a0)
        move.w  #$0041,BLTSIZE(a0)     ; height=1,width=1 -- starts the blit

.wait:  move.w  DMACONR(a0),d0
        btst    #14,d0                 ; BBUSY
        bne.s   .wait

        DONE
