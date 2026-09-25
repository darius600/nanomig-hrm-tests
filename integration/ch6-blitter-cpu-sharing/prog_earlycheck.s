; prog_earlycheck.s -- A3 [p187]: a small blit can finish before the
; CPU's first busy-bit check. Starts a blit, performs the manual's
; prescribed one dummy chip-memory read, then immediately reads and
; stores DMACONR. This file alone (no symbol set) is the SMALL build
; (one word). prog_earlycheck_large.s (EARLY_LARGE set) sizes the blit
; to take many more cycles, so the same immediate check finds it still
; busy. prog_earlycheck_reordered.s (EARLY_REORDER set, A3PRE's
; mutation) performs the dummy read before the blit is even started, so
; its own destination write cannot precede it in ram_log().

        include "nanomig.i"

EC_SRC  equ     $0C2000
EC_DST  equ     $0C2100

        org     PROGRAM_ORG

start:
        ifd     EARLY_REORDER
        move.w  RESULTS+2,d0           ; dummy read, before the blit is started at all
        endif

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #EC_SRC,BLTAPTH(a0)
        move.l  #EC_DST,BLTDPTH(a0)
        ifd     EARLY_LARGE
        move.w  #$3201,BLTSIZE(a0)     ; height=200,width=1 -- starts the blit
        else
        move.w  #$0041,BLTSIZE(a0)     ; height=1,width=1 -- starts the blit
        endif

        ifd     EARLY_REORDER
        else
        move.w  RESULTS+2,d0           ; the manual's prescribed dummy chip-memory read
        endif
        move.w  DMACONR(a0),RESULTS    ; immediate first busy-bit check, stored for grading

        DONE
